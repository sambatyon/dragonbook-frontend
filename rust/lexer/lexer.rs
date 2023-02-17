use std::collections::HashMap;
use std::io::{BufReader, ErrorKind, Read, Result};

pub mod tokens;
use tokens::{Tag, Token};

pub struct Lexer<T: std::io::Read> {
  pub line: u32,
  peek: u8,
  words: HashMap<String, Token>,
  reader: BufReader<T>,
}

impl<T: std::io::Read> Lexer<T> {
  pub fn new(source: BufReader<T>) -> Lexer<T> {
    let mut lexer = Lexer {
      line: 1,
      peek: b' ',
      words: HashMap::new(),
      reader: source,
    };
    // TODO(sambatyon): Check wether String can be replaced with &str
    lexer.words.insert(String::from("if"), Token::Word(String::from("if"), Tag::IF));
    lexer.words.insert(String::from("else"), Token::Word(String::from("else"), Tag::ELSE));
    lexer.words.insert(String::from("while"), Token::Word(String::from("while"), Tag::WHILE));
    lexer.words.insert(String::from("do"), Token::Word(String::from("do"), Tag::DO));
    lexer.words.insert(String::from("break"), Token::Word(String::from("break"), Tag::BREAK));
    lexer.words.insert(String::from("true"), Token::true_token().clone());
    lexer.words.insert(String::from("false"), Token::false_token().clone());
    lexer.words.insert(String::from("int"), Token::integer().clone());
    lexer.words.insert(String::from("float"), Token::float().clone());
    lexer.words.insert(String::from("char"), Token::ch().clone());
    lexer.words.insert(String::from("bool"), Token::boolean().clone());
    lexer
  }

  pub fn scan(&mut self) -> Result<Token> {
    if self.peek != b' ' {
      let tok = Token::Tok(self.peek);
      self.peek = b' ';
      return Ok(tok)
    }
    if self.peek == b'\0' {
      return Ok(Token::Eof);
    }
    loop {
      self.read()?;
      if self.peek == b' ' || self.peek == b'\t' || self.peek == b'\r' {
        continue;
      }
      if self.peek == b'\n' {
        self.line += 1;
        continue;
      }
      break;
    }

    match self.peek {
      b'&' => {
        return match self.read_ch(b'&') {
          Ok(true) => Ok(Token::And),
          Ok(false) => Ok(Token::Tok(b'&')),
          Err(error) => match error.kind() {
            ErrorKind::UnexpectedEof => Ok(Token::Tok(b'&')),
            _ => Err(error)
          }
        }
      },
      b'|' => {
        return match self.read_ch(b'|') {
          Ok(true) => Ok(Token::Or),
          Ok(false) => Ok(Token::Tok(b'|')),
          Err(error) => match error.kind() {
            ErrorKind::UnexpectedEof => Ok(Token::Tok(b'|')),
            _ => Err(error)
          }
        }
      },
      b'=' => {
        return match self.read_ch(b'=') {
          Ok(true) => Ok(Token::Equality),
          Ok(false) => Ok(Token::Tok(b'=')),
          Err(error) => match error.kind() {
            ErrorKind::UnexpectedEof => Ok(Token::Tok(b'=')),
            _ => Err(error)
          }
        }
      },
      b'!' => {
        return match self.read_ch(b'=') {
          Ok(true) => Ok(Token::Ne),
          Ok(false) => Ok(Token::Tok(b'!')),
          Err(error) => match error.kind() {
            ErrorKind::UnexpectedEof => Ok(Token::Tok(b'!')),
            _ => Err(error)
          }
        }
      },
      b'<' => {
        return match self.read_ch(b'=') {
          Ok(true) => Ok(Token::Le),
          Ok(false) => Ok(Token::Tok(b'<')),
          Err(error) => match error.kind() {
            ErrorKind::UnexpectedEof => Ok(Token::Tok(b'<')),
            _ => Err(error)
          }
        }
      },
      b'>' => {
        return match self.read_ch(b'=') {
          Ok(true) => Ok(Token::Ge),
          Ok(false) => Ok(Token::Tok(b'>')),
          Err(error) => match error.kind() {
            ErrorKind::UnexpectedEof => Ok(Token::Tok(b'>')),
            _ => Err(error)
          }
        }
      },
      _ => (),
    }

    if self.peek.is_ascii_digit() {
      let mut val: i64 = 0;
      loop {
        val = 10*val + ((self.peek - b'0') as i64);
        match self.read() {
          Err(error) => match error.kind() {
            ErrorKind::UnexpectedEof => break,
            _ => return Err(error)
          }
          _ => ()
        }
        if !self.peek.is_ascii_digit() {
          break
        }
      }

      if self.peek != b'.' {
        return Ok(Token::Integer(val))
      }

      let mut x = val as f64;
      let mut d = 10.0;
      loop {
        match self.read() {
          Err(error) => match error.kind() {
            ErrorKind::UnexpectedEof => break,
            _ => return Err(error)
          }
          _ => ()
        }
        if !self.peek.is_ascii_digit() {
          break
        }
        x += (self.peek - b'0') as f64 / d;
        d *= 10.0;
      }
      return Ok(Token::Real(x))
    }

    if self.peek.is_ascii_alphabetic() {
      let mut ident = String::new();
      loop {
        ident.push(self.peek as char);
        match self.read() {
          Err(error) => match error.kind() {
            ErrorKind::UnexpectedEof => break,
            _ => return Err(error)
          }
          _ => ()
        }
        if !self.peek.is_ascii_alphanumeric() {
          break
        }
      }
      if self.words.contains_key(&ident) {
        let w = self.words.get(&ident).ok_or(
          std::io::Error::new(
            ErrorKind::Other,
            format!("{} not in keywords", ident)))?;
        return Ok(w.clone());
      }
      return Ok(Token::from_str(ident.as_str()))
    }

    let tok = Token::Tok(self.peek);
    self.peek = b' ';
    Ok(tok)
  }

  fn read_ch(&mut self, c: u8) -> Result<bool> {
    self.read()?;
    if self.peek != c {
      return Ok(false);
    }
    self.peek = b' ';
    Ok(true)
  }

  fn read(&mut self) -> Result<()> {
    let mut buf = [0; 1];
    match self.reader.read_exact(&mut buf) {
      Ok(_) => (),
      Err(err) => match err.kind() {
        ErrorKind::UnexpectedEof => {
          self.peek = b'\0';
          return Ok(())
        }
        _ => return Err(err)
      }
    }
    self.peek = buf[0];
    Ok(())
  }
}

#[cfg(test)]
mod test {
use super::*;
use stringreader::StringReader;
use tokens::Tag;

fn tok(c: u8) -> Token {
  Token::Tok(c)
}

fn word(s: &str) -> Token {
  Token::Word(s.to_string(), Tag::ID)
}

fn simple(s: &str, w: u8) -> Token {
  Token::SimpleType(s.to_string(), w)
}

fn int(i: i64) -> Token {
  Token::Integer(i)
}

fn float(f: f64) -> Token {
  Token::Real(f)
}

fn while_kwd() -> Token {
  Token::Word(String::from("while"), Tag::WHILE)
}

fn do_kwd() -> Token {
  Token::Word(String::from("do"), Tag::DO)
}

fn if_kwd() -> Token {
  Token::Word(String::from("if"), Tag::IF)
}

fn break_kwd() -> Token {
  Token::Word(String::from("break"), Tag::BREAK)
}

fn true_kwd() -> Token {
  Token::Word(String::from("true"), Tag::TRUE)
}

#[test]
fn lexer_tests() {
  let tests: Vec<(&str, Vec<Token>)> = vec![
    ("&", vec![tok(b'&')]),
    ("&&", vec![Token::And]),
    ("|", vec![tok(b'|')]),
    ("||", vec![Token::Or]),
    ("=", vec![tok(b'=')]),
    ("==", vec![Token::Equality]),
    ("!", vec![tok(b'!')]),
    ("!=", vec![Token::Ne]),
    ("<", vec![tok(b'<')]),
    ("<=", vec![Token::Le]),
    (">", vec![tok(b'>')]),
    (">=", vec![Token::Ge]),
    ("1982", vec![int(1982)]),
    ("1982.2891", vec![float(1982.2891)]),
    ("Iden7ifier23", vec![word("Iden7ifier23")]),
    ("{
        int i; int j; float v; float[100] a;
        while (true) {
          do i = i + 1; while(a[i] < v);
          do j = j - 1; while(a[j] > v);
          if (i >= j) break;
          x = a[i];
          a[i] = a[j];
          a[j] = x;
        }
      }",
    vec![
      tok(b'{'),
      simple("int", 4u8), word("i"), tok(b';'),
      simple("int", 4u8), word("j"), tok(b';'),
      simple("float", 8u8), word("v"), tok(b';'),
      simple("float", 8u8), tok(b'['), int(100), tok(b']'), word("a"), tok(b';'),
      while_kwd(), tok(b'('), true_kwd(), tok(b')'), tok(b'{'),
      do_kwd(), word("i"), tok(b'='), word("i"), tok(b'+'), int(1), tok(b';'),
      while_kwd(), tok(b'('), word("a"), tok(b'['), word("i"), tok(b']'), tok(b'<'), word("v"), tok(b')'), tok(b';'),
      do_kwd(), word("j"), tok(b'='), word("j"), tok(b'-'), int(1), tok(b';'),
      while_kwd(), tok(b'('), word("a"), tok(b'['), word("j"), tok(b']'), tok(b'>'), word("v"), tok(b')'), tok(b';'),
      if_kwd(), tok(b'('), word("i"), Token::Ge, word("j"), tok(b')'),
      break_kwd(), tok(b';'),
      word("x"), tok(b'='), word("a"), tok(b'['), word("i"), tok(b']'), tok(b';'),
      word("a"), tok(b'['), word("i"), tok(b']'), tok(b'='), word("a"), tok(b'['), word("j"), tok(b']'), tok(b';'),
      word("a"), tok(b'['), word("j"), tok(b']'), tok(b'='), word("x"), tok(b';'),
      tok(b'}'),
      tok(b'}')]),
  ];

  for tc in tests {
    let mut lexer = Lexer::new(BufReader::new(StringReader::new(tc.0)));
    for expected in tc.1 {
      let tok = lexer.scan().unwrap();
      assert_eq!(tok, expected);
    }
  }
}
}
