use std::fmt;
use lexer::tokens;

use super::Type;
use super::emit_jumps;

pub trait Expression: fmt::Display {
  fn op(&self) -> tokens::Token;
  fn typ(&self) -> Type;
  fn generate(&self, b: &mut String) -> Result<Box<dyn Expression>, String>;
  fn reduce(&self, b: &mut String) -> Result<Box<dyn Expression>, String>;
  fn jumps(&self, b: &mut String, to: i64, from: i64) -> Result<(), String>;
}

pub struct Constant {
  token: tokens::Token,
  typ: Type
}

impl Constant {
  fn integer(tok: tokens::Token) -> Result<Constant, String> {
    match tok {
      tokens::Token::Integer(_) => Ok(Constant{token: tok, typ: Type::integer()}),
      t => Err(format!("Invalid parameter: {}", t))
    }
  }

  fn float(tok: tokens::Token) -> Result<Constant, String> {
    match tok {
      tokens::Token::Real(_) => Ok(Constant{token: tok, typ: Type::float()}),
      t => Err(format!("Invalid parameter: {}", t))
    }
  }
}

#[derive(Clone)]
pub struct Identifier {
  id: tokens::Token,
  typ: Type,
  offset: i32,
}

impl Identifier {

}

impl Expression for Identifier {
  fn op(&self) -> tokens::Token {
    self.id.clone()
  }

  fn typ(&self) -> Type {
    self.typ.clone()
  }

  fn generate(&self, b: &mut String) -> Result<Box<dyn Expression>, String> {
    Ok(Box::new(self.clone()))
  }

  fn reduce(&self, b: &mut String) -> Result<Box<dyn Expression>, String> {
    Ok(Box::new(self.clone()))
  }

  fn jumps(&self, b: &mut String, to: i64, from: i64) -> Result<(), String> {
    emit_jumps(b, format!("{}", self).as_str(), to, from);
    Ok(())
  }
}

impl fmt::Display for Identifier {
  fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
    write!(f, "{}", self.id)
  }
}
