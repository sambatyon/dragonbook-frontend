use std::fmt;

use super::Type;
use super::emit;
use super::expression::{Identifier, Expression};

pub trait Statement: fmt::Display {
  fn generate(&self, b: &mut String, begin: i64, after: i64) -> Result<(), String>;
  fn after(&self) -> i64;
}

pub struct NullStmt {}

impl NullStmt {
  pub fn new() -> NullStmt {
    NullStmt { }
  }
}

impl Statement for NullStmt {
  fn generate(&self, b: &mut String, being: i64, after: i64) -> Result<(), String> {
      Ok(())
  }

  fn after(&self) -> i64 {
      0
  }
}

impl fmt::Display for NullStmt {
  fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
    write!(f, "")
  }
}

pub struct AssignStmt {
  id: Box<Identifier>,
  expr: Box<dyn Expression>,
}

impl AssignStmt {
  pub fn new(id: Box<Identifier>, expr: Box<dyn Expression>) -> Result<AssignStmt, String> {
    let idt = id.typ();
    let ext = expr.typ();
    if idt.is_numeric() && !ext.is_numeric() || !idt.is_numeric() && ext.is_numeric() {
      return Err(String::from("Type Error"));
    }
    let bt = Type::boolean();
    if idt == bt && ext != bt || idt != bt && ext == bt {
      return Err(String::from("Type Error"));
    }
    Ok(AssignStmt { id: id, expr: expr })
  }
}

impl Statement for AssignStmt {
  fn generate(&self, b: &mut String, begin: i64, after: i64) -> Result<(), String> {
    let expr = self.expr.generate(b)?;
    emit(b, format!("{}", self).as_str());
    Ok(())
  }

  fn after(&self) -> i64 {
    0
  }
}

impl fmt::Display for AssignStmt {
  fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
    write!(f, "{} = {}", self.id, self.expr)
  }
}

#[cfg(test)]
mod test {
use crate::{reset_labels, new_label};
use crate::expression::{Temp, Constant};

use lexer::tokens::Tag;
use lexer::tokens::Token;

use super::*;

#[test]
fn statement_tests() {
  let tests: Vec<(Box<dyn Statement>, &str)> = vec![
    (
      Box::new(AssignStmt::new(
        Box::new(Identifier::new(Token::Word(String::from("x"), Tag::ID), Type::integer(), 4)),
        Box::new(Constant::integer(42)),
      ).unwrap()),
      "\tx = 42\n",
    )
  ];

  for tc in tests {
    reset_labels();
    Temp::reset_counter();

    let begin = new_label();
    let after = new_label();
    let mut b = String::new();
    tc.0.generate(&mut b, begin, after);
    assert_eq!(tc.1, b);
  }
}
}
