use std::collections::HashMap;
use lexer::tokens as toks;
use inter::expression as expr;
use inter::statement as stmt;

pub struct Environment {
  table: HashMap<String, expr::Identifier>,
  previous: Box<Option<Environment>>,
}

impl Environment {
  fn new() -> Box<Environment> {
    Box::new(Environment { table: HashMap::new(), previous: Box::new(None) })
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
  label_mngr: inter::LabelManager,
  used: i64,
  encstmt: Box<dyn stmt::Statement>,
}

impl<T: std::io::Read> Parser<T> {
  pub fn new(lexer: lexer::Lexer<T>) -> Parser<T> {
    Parser {
      lexer: lexer,
      lookahead: toks::Token::Eof,
      top: Environment::new(),
      label_mngr: inter::LabelManager::new(),
      used: 0,
      encstmt: Box::new(stmt::NullStmt::new())
    }
  }

  pub fn program(s: &mut String) -> Result<(), String> {
    Ok(())
  }

  fn next(&mut self) -> Result<(), String> {
    self.lookahead = match self.lexer.scan() {
      Ok(tok) => tok,
      Err(err) => return Err(format!("{} near line {}", err, self.lexer.line))
    };
    Ok(())
  }

  fn match_tokens(&mut self, tag: toks::Tag) -> Result<(), String> {
    if self.lookahead.tag() != tag as u32 {
      return Err(format!("Syntax error near line {}", self.lexer.line))
    }
    self.next()
  }
}
