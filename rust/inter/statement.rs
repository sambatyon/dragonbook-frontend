use crate::emit_label;
use crate::expression::AccessOp;
use crate::new_label;

use super::Type;
use super::emit;
use super::expression::{Identifier, Expression};

pub trait Statement {
  // TODO(sambatyon): This should take a label generator
  fn generate(&mut self, b: &mut String, begin: i64, after: i64) -> Result<(), String>;
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

  pub fn new_box() -> Box<NullStmt> {
    Box::new(NullStmt::new())
  }
}

impl Statement for NullStmt {
  fn generate(&mut self, b: &mut String, being: i64, after: i64) -> Result<(), String> {
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

  pub fn new_box(id: Box<Identifier>, expr: Box<dyn Expression>) -> Result<Box<AssignStmt>, String> {
    let ass = AssignStmt::new(id, expr)?;
    Ok(Box::new(ass))
  }
}

impl Statement for AssignStmt {
  fn generate(&mut self, b: &mut String, begin: i64, after: i64) -> Result<(), String> {
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

  pub fn new_box(access: Box<AccessOp>, expr: Box<dyn Expression>) -> Result<Box<AssingArrayStmt>, String> {
    let aas = AssingArrayStmt::new(access, expr)?;
    Ok(Box::new(aas))
  }
}

impl Statement for AssingArrayStmt {
  fn generate(&mut self, b: &mut String, begin: i64, after: i64) -> Result<(), String> {
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

  fn new_box(head: Box<dyn Statement>, tail: Box<dyn Statement>) -> Box<StmtSeq> {
    Box::new(StmtSeq::new(head, tail))
  }
}

impl Statement for StmtSeq {
  fn generate(&mut self, b: &mut String, begin: i64, after: i64) -> Result<(), String> {
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

  fn new_box(cond: Box<dyn Expression>, body: Box<dyn Statement>) -> Result<Box<IfStmt>, String> {
    let is = IfStmt::new(cond, body)?;
    Ok(Box::new(is))
  }
}

impl Statement for IfStmt {
  fn generate(&mut self, b: &mut String, begin: i64, after: i64) -> Result<(), String> {
    let label = new_label();
    self.cond.jumps(b, 0, after)?;
    emit_label(b, label);
    self.body.generate(b, label, after)
  }
}

struct ElseStmt {
  cond: Box<dyn Expression>,
  true_stmt: Box<dyn Statement>,
  false_stmt: Box<dyn Statement>,
}

impl ElseStmt {
  pub fn new(cond: Box<dyn Expression>, true_stmt: Box<dyn Statement>, false_stmt: Box<dyn Statement>) -> Result<ElseStmt, String> {
    if cond.typ() != Type::boolean() {
      return Err(String::from("If condition should be of bool type"))
    }
    Ok(ElseStmt { cond: cond, true_stmt: true_stmt, false_stmt: false_stmt })
  }

  pub fn new_box(cond: Box<dyn Expression>, true_stmt: Box<dyn Statement>, false_stmt: Box<dyn Statement>) -> Result<Box<ElseStmt>, String> {
    let es = ElseStmt::new(cond, true_stmt, false_stmt)?;
    Ok(Box::new(es))
  }
}

impl Statement for ElseStmt {
  fn generate(&mut self, b: &mut String, begin: i64, after: i64) -> Result<(), String> {
    let label1 = new_label();
    let label2 = new_label();
    self.cond.jumps(b, 0, label2)?;
    emit_label(b, label1);
    self.true_stmt.generate(b, label1, after)?;
    emit_label(b, label2);
    self.false_stmt.generate(b, label2, after)
  }
}

struct WhileStmt {
  cond: Box<dyn Expression>,
  body: Box<dyn Statement>,
  after: i64,
}

impl WhileStmt {
  fn new(cond: Box<dyn Expression>, body: Box<dyn Statement>) -> Result<WhileStmt, String> {
    if cond.typ() != Type::boolean() {
      return Err(String::from("While condition should be of bool type"))
    }
    Ok(WhileStmt { cond: cond, body: body, after: 0 })
  }

  fn new_box(cond: Box<dyn Expression>, body: Box<dyn Statement>) -> Result<Box<WhileStmt>, String> {
    let ws = WhileStmt::new(cond, body)?;
    Ok(Box::new(ws))
  }
}

impl Statement for WhileStmt {
  fn generate(&mut self, b: &mut String, begin: i64, after: i64) -> Result<(), String> {
    self.after = after;
    self.cond.jumps(b, 0, after)?;
    let label = new_label();
    emit_label(b, label);
    self.body.generate(b, label, begin)?;
    emit(b, format!("goto L{}", begin).as_str());
    Ok(())
  }

  fn after(&self) -> i64 {
    self.after
  }
}

struct DoStmt {
  cond: Box<dyn Expression>,
  body: Box<dyn Statement>,
  after: i64,
}

impl DoStmt {
  fn new(cond: Box<dyn Expression>, body: Box<dyn Statement>) -> Result<DoStmt, String> {
    if cond.typ() != Type::boolean() {
      return Err(String::from("While condition should be of bool type"))
    }
    Ok(DoStmt { cond: cond, body: body, after: 0 })
  }

  fn new_box(cond: Box<dyn Expression>, body: Box<dyn Statement>) -> Result<Box<DoStmt>, String> {
    let ws = DoStmt::new(cond, body)?;
    Ok(Box::new(ws))
  }
}

impl Statement for DoStmt {
  fn generate(&mut self, b: &mut String, begin: i64, after: i64) -> Result<(), String> {
    self.after = after;
    let label = new_label();
    self.body.generate(b, begin, label)?;
    emit_label(b, label);
    self.cond.jumps(b, begin, 0)
  }

  fn after(&self) -> i64 {
    self.after
  }
}

struct BreakStmt {
  enclosing: Box<dyn Statement>
}

impl BreakStmt {
  fn new(enclosing: Box<dyn Statement>) -> Result<BreakStmt, String> {
    if enclosing.is_null() {
      return Err(String::from("Unenclosed break"))
    }
    Ok(BreakStmt { enclosing: enclosing })
  }

  fn new_box(enclosing: Box<dyn Statement>) -> Result<Box<BreakStmt>, String> {
    let bs = BreakStmt::new(enclosing)?;
    Ok(Box::new(bs))
  }
}

impl Statement for BreakStmt {
  fn generate(&mut self, b: &mut String, begin: i64, after: i64) -> Result<(), String> {
    emit(b, format!("goto L{}", self.enclosing.after()).as_str());
    Ok(())
  }
}

#[cfg(test)]
mod test {
use crate::{reset_labels, new_label};
use crate::expression::{Temp, Constant};

use lexer::tokens::{Tag, Token};

use super::*;

#[test]
fn statement_tests() {
  let mut tests: Vec<(Box<dyn Statement>, &str)> = vec![
    (
      AssignStmt::new_box(
        Identifier::new_box(Token::from_str("x"), Type::integer(), 4),
        Box::new(Constant::integer(42)),
      ).unwrap(),
      "\tx = 42\n",
    ),
    (
      AssingArrayStmt::new_box(
        AccessOp::new_box(
          Identifier::new_box(Token::from_str("arr"), Type::float(), 4),
          Identifier::new_box(Token::from_str("x"), Type::integer(), 4),
          Type::float()
        ),
        Box::new(Constant::float(42.0)),
      ).unwrap(),
      "\tarr [ x ] = 42\n",
    ),
    (
      StmtSeq::new_box(
        AssignStmt::new_box(
          Identifier::new_box(Token::from_str("x"), Type::integer(), 4),
          Box::new(Constant::integer(42)),
        ).unwrap(),
        AssingArrayStmt::new_box(
          AccessOp::new_box(
            Identifier::new_box(Token::from_str("arr"), Type::float(), 4),
            Identifier::new_box(Token::from_str("x"), Type::integer(), 4),
            Type::float()
          ),
          Box::new(Constant::float(42.0)),
        ).unwrap()
      ),
      "\tx = 42\nL3:\tarr [ x ] = 42\n"
    ),
    (
      IfStmt::new_box(
        Identifier::new_box(Token::from_str("b"), Type::boolean(), 4),
        AssignStmt::new_box(
          Identifier::new_box(Token::from_str("x"), Type::integer(), 4),
          Box::new(Constant::integer(0)),
        ).unwrap()
      ).unwrap(),
      "\tiffalse b goto L2\nL3:\tx = 0\n"
    ),
    (
      ElseStmt::new_box(
        Identifier::new_box(Token::from_str("b"), Type::boolean(), 4),
        AssignStmt::new_box(
          Identifier::new_box(Token::from_str("x"), Type::integer(), 4),
          Box::new(Constant::integer(0)),
        ).unwrap(),
        AssignStmt::new_box(
          Identifier::new_box(Token::from_str("x"), Type::integer(), 4),
          Box::new(Constant::integer(42)),
        ).unwrap(),
      ).unwrap(),
      "\tiffalse b goto L4\nL3:\tx = 0\nL4:\tx = 42\n",
    ),
    (
      WhileStmt::new_box(
        Identifier::new_box(Token::from_str("b"), Type::boolean(), 4),
        AssignStmt::new_box(
          Identifier::new_box(Token::from_str("x"), Type::integer(), 4),
          Box::new(Constant::integer(0)),
        ).unwrap(),
      ).unwrap(),
      "\tiffalse b goto L2\nL3:\tx = 0\n\tgoto L1\n"
    ),
    (
      DoStmt::new_box(
        Identifier::new_box(Token::from_str("b"), Type::boolean(), 4),
        AssignStmt::new_box(
          Identifier::new_box(Token::from_str("x"), Type::integer(), 4),
          Box::new(Constant::integer(0)),
        ).unwrap(),
      ).unwrap(),
      "\tx = 0\nL3:\tif b goto L1\n"
    ),
  ];

  for mut tc in tests {
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
