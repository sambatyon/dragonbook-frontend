use std::io::BufReader;
use std::io::Read;
use std::io::Result;
use std::io::ErrorKind;

pub mod tokens;
use tokens::Token;

pub struct Lexer<T: std::io::Read> {
  line: u32,
  peek: u8,
  reader: BufReader<T>,
}

impl<T: std::io::Read> Lexer<T> {
  pub fn new(source: T) -> Lexer<T> {
    return Lexer {
      line: 1,
      peek: b' ',
      reader: BufReader::new(source),
    };
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
      return Ok(Token::from_string(ident))
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
          return Err(err)
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


#[test]
fn lexer_tests() {
  let tests: Vec<(&str, Vec<Token>)> = vec![
    ("&", vec![Token::Tok(b'&')]),
    ("&&", vec![Token::And]),
    ("|", vec![Token::Tok(b'|')]),
    ("||", vec![Token::Or]),
    ("=", vec![Token::Tok(b'=')]),
    ("==", vec![Token::Equality]),
    ("!", vec![Token::Tok(b'!')]),
    ("!=", vec![Token::Ne]),
    ("<", vec![Token::Tok(b'<')]),
    ("<=", vec![Token::Le]),
    (">", vec![Token::Tok(b'>')]),
    (">=", vec![Token::Ge]),
    ("1982", vec![Token::Integer(1982)]),
    ("1982.2891", vec![Token::Real(1982.2891)]),
    ("Iden7ifier23", vec![Token::Word("Iden7ifier23".to_string(), Tag::ID)]),
    ("{
        i;
      }",
    vec![
      Token::Tok(b'{'),
      Token::Word("i".to_string(), Tag::ID), Token::Tok(b';'),
      Token::Tok(b'}')]),
  ];

  for tc in tests {
    let mut lexer = Lexer::new(StringReader::new(tc.0));
    for expected in tc.1 {
      let tok = lexer.scan().unwrap();
      assert_eq!(tok, expected);
    }
  }
}
}
