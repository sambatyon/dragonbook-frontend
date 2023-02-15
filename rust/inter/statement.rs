use crate::emit_label;
use crate::expression::AccessOp;
use crate::new_label;

use super::Type;
use super::emit;
use super::expression::{Identifier, Expression};

pub trait Statement {
  fn generate(&self, b: &mut String, begin: i64, after: i64) -> Result<(), String>;
  fn after(&self) -> i64 {
    0
  }
  fn is_null(&self) -> bool {
    false
  }
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
  fn is_null(&self) -> bool {
      true
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
    emit(b, format!("{} = {}", self.id, self.expr).as_str());
    Ok(())
  }
}

fn check_types(tleft: &Type, tright: &Type) -> Option<Type> {
  match tleft {
    Type::Array { of, length } => return None,
    _ => ()
  };
  match tright {
    Type::Array { of, length } => return None,
    _ => ()
  };
  if tleft == tright || (tleft.is_numeric() && tright.is_numeric()) {
    return Some(tright.clone());
  }
  None
}

pub struct AssingArrayStmt {
  id: Box<Identifier>,
  index: Box<dyn Expression>,
  expr: Box<dyn Expression>,
}

impl AssingArrayStmt {
  pub fn new(access: Box<AccessOp>, expr: Box<dyn Expression>) -> Result<AssingArrayStmt, String> {
    if check_types(&access.typ(), &expr.typ()).is_none() {
      return Err(String::from("Type Error"))
    }
    Ok(AssingArrayStmt {
      id: access.array.clone(),
      index: access.index.box_clone(),
      expr: expr.box_clone()
    })
  }
}

impl Statement for AssingArrayStmt {
  fn generate(&self, b: &mut String, begin: i64, after: i64) -> Result<(), String> {
    let idx = self.index.reduce(b)?;
    let expr = self.expr.reduce(b)?;
    emit(b, format!("{} [ {} ] = {}", self.id, self.index, self.expr).as_str());
    Ok(())
  }
}

pub struct StmtSeq {
  head: Box<dyn Statement>,
  tail: Box<dyn Statement>,
}

impl StmtSeq {
  fn new(head: Box<dyn Statement>, tail: Box<dyn Statement>) -> StmtSeq {
    StmtSeq { head: head, tail: tail }
  }
}

impl Statement for StmtSeq {
  fn generate(&self, b: &mut String, begin: i64, after: i64) -> Result<(), String> {
    if self.head.is_null() {
      return self.tail.generate(b, begin, after);
    }
    if self.tail.is_null() {
      return self.head.generate(b, begin, after);
    }
    let label = new_label();
    self.head.generate(b, begin, label)?;
    emit_label(b, label);
    self.tail.generate(b, label, after)
  }
}

struct IfStmt {
  cond: Box<dyn Expression>,
  body: Box<dyn Statement>,
}

impl IfStmt {
  fn new(cond: Box<dyn Expression>, body: Box<dyn Statement>) -> Result<IfStmt, String> {
    if cond.typ() != Type::boolean() {
      return Err(String::from("If condition should be of bool type"))
    }
    Ok(IfStmt {cond: cond, body: body})
  }
}

impl Statement for IfStmt {
  fn generate(&self, b: &mut String, begin: i64, after: i64) -> Result<(), String> {
    let label = new_label();
    self.cond.jumps(b, 0, after)?;
    emit_label(b, label);
    self.body.generate(b, label, after)
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
        Box::new(Identifier::new(Token::from_str("x"), Type::integer(), 4)),
        Box::new(Constant::integer(42)),
      ).unwrap()),
      "\tx = 42\n",
    ),
    (
      Box::new(AssingArrayStmt::new(
        Box::new(AccessOp::new(
          Box::new(Identifier::new(Token::from_str("arr"), Type::float(), 4)),
          Box::new(Identifier::new(Token::from_str("x"), Type::integer(), 4)),
          Type::float()
        )),
        Box::new(Constant::float(42.0)),
      ).unwrap()),
      "\tarr [ x ] = 42\n",
    ),
    (
      Box::new(StmtSeq::new(
        Box::new(AssignStmt::new(
          Box::new(Identifier::new(Token::from_str("x"), Type::integer(), 4)),
          Box::new(Constant::integer(42)),
        ).unwrap()),
        Box::new(AssingArrayStmt::new(
          Box::new(AccessOp::new(
            Box::new(Identifier::new(Token::from_str("arr"), Type::float(), 4)),
            Box::new(Identifier::new(Token::from_str("x"), Type::integer(), 4)),
            Type::float()
          )),
          Box::new(Constant::float(42.0)),
        ).unwrap())
      )),
      "\tx = 42\nL3:\tarr [ x ] = 42\n"
    ),
    (
      Box::new(IfStmt::new(
        Box::new(Identifier::new(Token::from_str("b"), Type::boolean(), 4)),
        Box::new(AssignStmt::new(
          Box::new(Identifier::new(Token::from_str("x"), Type::integer(), 4)),
          Box::new(Constant::integer(0)),
        ).unwrap())
      ).unwrap()),
      "\tiffalse b goto L2\nL3:\tx = 0\n"
    ),
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
