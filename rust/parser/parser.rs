use std::collections::HashMap;
use std::mem::swap;
use lexer::tokens as toks;
use inter::expression as expr;
use inter::statement as stmt;

pub struct Environment {
  table: HashMap<String, expr::Identifier>,
  previous: Box<Option<Environment>>,
}

impl Environment {
  fn empty() -> Box<Environment> {
    Box::new(Environment { table: HashMap::new(), previous: Box::new(None) })
  }

  fn new(prev: Box<Environment>) -> Box<Environment> {
    Box::new(Environment { table: HashMap::new(), previous: Box::new(Some(*prev)) })
  }

  fn put(&mut self, key: &str, value: expr::Identifier) {
    self.table.insert(key.to_string(), value);
  }

  fn get(&self, key: &str) -> Option<expr::Identifier> {
    match self.table.get(key) {
      Some(value) => Some(value.clone()),
      _ => match self.previous.as_ref() {
        Some(env) => env.get(key),
        _ => None
      }
    }
  }
}

struct Parser<T: std::io::Read> {
  lexer: lexer::Lexer<T>,
  lookahead: toks::Token,
  top: Box<Environment>,
  used: i64,
  encstmt: Box<dyn stmt::Statement>,
}

impl<T: std::io::Read> Parser<T> {
  pub fn new(lexer: lexer::Lexer<T>) -> Result<Parser<T>, String> {
    let mut res = Parser {
      lexer: lexer,
      lookahead: toks::Token::Eof,
      top: Environment::empty(),
      used: 0,
      encstmt: Box::new(stmt::NullStmt::new())
    };
    res.next()?;
    Ok(res)
  }

  pub fn program(&mut self, s: &mut String) -> Result<(), String> {
    let mut stm = self.block(s)?;
    let begin = inter::new_label();
    let after = inter::new_label();
    inter::emit_label(s, begin);
    stm.generate(s, begin, after)?;
    inter::emit_label(s, after);
    Ok(())
  }

  fn next(&mut self) -> Result<(), String> {
    self.lookahead = match self.lexer.scan() {
      Ok(tok) => tok,
      Err(err) => return Err(format!("{} near line {}", err, self.lexer.line))
    };
    Ok(())
  }

  fn match_tokens(&mut self, tag: u32) -> Result<(), String> {
    if self.lookahead.tag() != tag as u32 {
      return Err(format!("Syntax error near line {}", self.lexer.line))
    }
    self.next()
  }

  fn block(&mut self, s: &mut String) -> Result<Box<dyn stmt::Statement>, String> {
    self.match_tokens(b'{' as u32)?;

    let mut saved = Environment::empty();
    swap(&mut saved, &mut self.top);
    self.top.previous = Box::new(Some(*saved));

    self.decls(s)?;
    let stmts = self.stmts(s)?;
    self.match_tokens(b'}' as u32)?;

    let mut saved = Box::new(None);
    swap(&mut saved, &mut self.top.previous);
    let mut saved = Box::new(saved.expect("Expected environment"));
    swap(&mut saved, &mut self.top);
    Ok(stmts)
  }

  fn decls(&mut self, s: &mut String) -> Result<(), String> {
    while self.lookahead.tag() == toks::Tag::BASIC.as_u32() {
      let typ = self.typ()?;
      let tok = self.lookahead.clone();
      self.match_tokens(toks::Tag::ID.as_u32())?;
      self.match_tokens(b';' as u32)?;
      let id = expr::Identifier::new(tok, &typ, self.used as i32);
      self.top.put(id.to_string().as_str(), id);
      self.used += typ.width() as i64;
    }
    Ok(())
  }

  fn typ(&mut self) -> Result<inter::Type, String> {
    let typ = inter::Type::new(&self.lookahead)?;
    self.match_tokens(toks::Tag::BASIC.as_u32())?;
    if self.lookahead.tag() != (b'[' as u32) {
      return Ok(typ)
    }
    self.dims(typ)
  }

  fn dims(&mut self, typ: inter::Type) -> Result<inter::Type, String> {
    self.match_tokens(b'[' as u32)?;
    let tok = self.lookahead.clone();
    self.match_tokens(toks::Tag::INTEGER.as_u32())?;
    let size = match tok {
      toks::Token::Integer(val) => val,
      _ => return Err(format!("Syntax error near line {}", self.lexer.line))
    };
    self.match_tokens(b']' as u32)?;

    let mut of = typ.clone();
    if self.lookahead.tag() == (b'['  as u32) {
      of = self.dims(typ)?;
    }
    Ok(inter::Type::array(of, size as u32))
  }

  fn stmts(&mut self, s: &mut String) -> Result<Box<dyn stmt::Statement>, String> {
    if self.lookahead.tag() == '}' as u32 {
      return Ok(stmt::NullStmt::new_box())
    }
    let head = self.stmt(s)?;
    let tail = self.stmts(s)?;
    Ok(stmt::StmtSeq::new_box(head, tail))
  }

  fn stmt(&mut self, s: &mut String) -> Result<Box<dyn stmt::Statement>, String> {
    match self.lookahead.tag() {
      _ => self.assign(s)
    }
  }

  fn assign(&mut self, s: &mut String) -> Result<Box<dyn stmt::Statement>, String> {
    Err(String::from("Unimplemented"))
  }
}

#[cfg(test)]
mod test {
use super::*;

use std::io::BufReader;
use stringreader::StringReader;

#[test]
fn parser_tests() {
  let mut tests: Vec<(&str, &str)> = vec![
    ("{}", "L1:L2:"),
    ("{int i;}", "L1:L2:"),
    ("{int i;float f;bool[100] b;}", "L1:L2:"),
  ];

  for tc in tests {
    inter::reset_labels();
    expr::Temp::reset_counter();

    let mut lexer = lexer::Lexer::new(
      BufReader::new(StringReader::new(tc.0))
    );
    let mut parser = Parser::new(lexer).expect("Creating parser");

    let mut str = String::new();
    parser.program(&mut str).expect("Parsing program");

    assert_eq!(str, tc.1);
  }
}
}
