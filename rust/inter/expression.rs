use std::fmt;
use std::sync::atomic::{AtomicI32, Ordering};
use lexer::tokens::Token;
use lexer;

use super::Type;
use super::emit;
use super::emit_jumps;
use super::emit_label;
use super::new_label;

pub trait Expression: fmt::Display {
  fn op(&self) -> Token;
  fn typ(&self) -> Type;
  fn generate(&self, b: &mut String) -> Result<Box<dyn Expression>, String>;
  fn reduce(&self, b: &mut String) -> Result<Box<dyn Expression>, String>;
  fn jumps(&self, b: &mut String, to: i64, from: i64) -> Result<(), String>;

  fn box_clone(&self) -> Box<dyn Expression>;
}

#[derive(Clone,Eq)]
pub struct Constant {
  token: Token,
  typ: Type
}

impl Expression for Constant {
  fn op(&self) -> Token {
    self.token.clone()
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
    if self == Constant::true_constant() && to != 0 {
      emit(b, format!("goto L{}", to).as_str())
    } else if self == Constant::false_constant() && from != 0 {
      emit(b, format!("goto L{}", from).as_str())
    }
    Ok(())
  }

  fn box_clone(&self) -> Box<dyn Expression> {
    Box::new(self.clone())
  }
}

impl fmt::Display for Constant {
  fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
    write!(f, "{}", self.token)
  }
}

impl PartialEq<Constant> for Constant {
  fn eq(&self, other: &Self) -> bool {
    self.token == other.token && self.typ == other.typ
  }
}

impl PartialEq<Constant> for &Constant {
  fn eq(&self, other: &Constant) -> bool {
    (*self).token == other.token && (*self).typ == other.typ
  }
}

impl Constant {
  pub fn integer(tok: Token) -> Result<Constant, String> {
    match tok {
      Token::Integer(_) => Ok(Constant{token: tok, typ: Type::integer()}),
      t => Err(format!("Invalid parameter: {}", t))
    }
  }

  pub fn float(tok: Token) -> Result<Constant, String> {
    match tok {
      Token::Real(_) => Ok(Constant{token: tok, typ: Type::float()}),
      t => Err(format!("Invalid parameter: {}", t))
    }
  }

  pub fn true_constant() -> Constant {
    Constant{token: Token::true_token(), typ: Type::boolean()}
  }

  pub fn false_constant() -> Constant {
    Constant{token: Token::false_token(), typ: Type::boolean()}
  }
}

#[derive(Clone)]
pub struct Identifier {
  id: Token,
  typ: Type,
  offset: i32,
}

impl Identifier {
  pub fn new(id: Token, typ: Type, offset: i32) -> Identifier {
    Identifier { id: id, typ: typ, offset: offset }
  }
}

impl Expression for Identifier {
  fn op(&self) -> Token {
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

  fn box_clone(&self) -> Box<dyn Expression> {
    Box::new(self.clone())
  }
}

impl fmt::Display for Identifier {
  fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
    write!(f, "{}", self.id)
  }
}

#[derive(Clone)]
struct Temp {
  op: Token,
  typ: Type,
  num: i32,
}

static temp_counter: AtomicI32 = AtomicI32::new(1);

impl Temp {
  fn new(typ: Type) -> Temp {
    Temp{op: Token::temp_word(), typ: typ, num: temp_counter.fetch_add(1, Ordering::Relaxed)}
  }

  fn reset_counter() {
    temp_counter.store(1, Ordering::Relaxed)
  }
}

impl Expression for Temp {
  fn op(&self) -> Token {
    self.op.clone()
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

  fn box_clone(&self) -> Box<dyn Expression> {
    Box::new(self.clone())
  }
}

impl fmt::Display for Temp {
  fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
    write!(f, "t{}", self.num)
  }
}

struct ArithmeticOp {
  op: Token,
  typ: Type,
  left: Box<dyn Expression>,
  right: Box<dyn Expression>,
}

impl ArithmeticOp {
  pub fn new(tok: Token, left: Box<dyn Expression>, right: Box<dyn Expression>) -> Result<ArithmeticOp, String> {
    let typ = match Type::max_type(&left.typ(), &right.typ()) {
      Some(t) => t,
      None => return Err(String::from("Type error"))
    };
    Ok(ArithmeticOp{ op: tok, typ: typ, left: left, right: right })
  }
}

impl Expression for ArithmeticOp {
  fn op(&self) -> Token {
    self.op.clone()
  }

  fn typ(&self) -> Type {
    self.typ.clone()
  }

  fn generate(&self, b: &mut String) -> Result<Box<dyn Expression>, String> {
    let lr = self.left.reduce(b)?;
    let rr = self.right.reduce(b)?;
    match ArithmeticOp::new(self.op.clone(), lr, rr) {
      Ok(ao) => Ok(Box::new(ao)),
      Err(s) => Err(s)
    }
  }

  fn reduce(&self, b: &mut String) -> Result<Box<dyn Expression>, String> {
    let x = self.generate(b)?;
    let tmp = Temp::new(self.typ());
    emit(b, format!("{} = {}", tmp, x).as_str());
    Ok(Box::new(tmp))
  }

  fn jumps(&self, b: &mut String, to: i64, from: i64) -> Result<(), String> {
    Ok(emit_jumps(b, format!("{}", self).as_str(), to, from))
  }

  fn box_clone(&self) -> Box<dyn Expression> {
    Box::new(self.clone())
  }
}

impl fmt::Display for ArithmeticOp {
  fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
    write!(f, "{} {} {}", self.left, self.op, self.right)
  }
}

impl Clone for ArithmeticOp {
  fn clone(&self) -> Self {
    ArithmeticOp {
      op: self.op.clone(),
      typ: self.typ.clone(),
      left: self.left.box_clone(),
      right: self.right.box_clone(),
    }
  }
}

struct UnaryOp {
  op: Token,
  typ: Type,
  rest: Box<dyn Expression>,
}

impl UnaryOp {
  pub fn new(op: Token, rest: Box<dyn Expression>) -> Result<UnaryOp, String> {
    let typ = match Type::max_type(&Type::integer(), &rest.typ()) {
      Some(typ) => typ,
      _ => return Err(String::from("Type Error"))
    };
    Ok(UnaryOp { op: op, typ: typ, rest: rest })
  }
}

impl Expression for UnaryOp {
  fn op(&self) -> Token {
    self.op.clone()
  }

  fn typ(&self) -> Type {
    self.typ.clone()
  }

  fn generate(&self, b: &mut String) -> Result<Box<dyn Expression>, String> {
    let rest = self.rest.reduce(b)?;
    let unary = UnaryOp::new(self.op.clone(), rest)?;
    Ok(Box::new(unary))
  }

  fn reduce(&self, b: &mut String) -> Result<Box<dyn Expression>, String> {
    let x = self.generate(b)?;
    let tmp = Temp::new(self.typ.clone());
    emit(b, format!("{} = {}", tmp, x).as_str());
    Ok(Box::new(tmp))
  }

  fn jumps(&self, b: &mut String, to: i64, from: i64) -> Result<(), String> {
    emit_jumps(b, format!("{}", self).as_str(), to, from);
    Ok(())
  }

  fn box_clone(&self) -> Box<dyn Expression> {
    Box::new(self.clone())
  }
}

impl fmt::Display for UnaryOp {
  fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
    write!(f, "{} {}", self.op, self.rest)
  }
}

impl Clone for UnaryOp {
  fn clone(&self) -> Self {
    UnaryOp {
      op: self.op.clone(),
      typ: self.typ.clone(),
      rest: self.rest.box_clone(),
    }
  }
}

struct AccessOp {
  array: Box<Identifier>,
  index: Box<dyn Expression>,
  typ: Type,
}

impl AccessOp {
  fn new(array: Box<Identifier>, index: Box<dyn Expression>, typ: Type) -> AccessOp {
    AccessOp { array: array, index: index, typ: typ }
  }
}

impl Expression for AccessOp {
  fn op(&self) -> Token {
    lexer::tokens::Token::access_word().clone()
  }

  fn typ(&self) -> Type {
    self.typ.clone()
  }

  fn generate(&self, b: &mut String) -> Result<Box<dyn Expression>, String> {
    let idx = self.index.generate(b)?;
    Ok(Box::new(AccessOp::new(self.array.clone(), idx, self.typ.clone())))
  }

  fn reduce(&self, b: &mut String) -> Result<Box<dyn Expression>, String> {
    let x = self.generate(b)?;
    let tmp = Temp::new(self.typ());
    emit(b, format!("{} = {}", tmp, x).as_str());
    Ok(Box::new(tmp))
  }

  fn jumps(&self, b: &mut String, to: i64, from: i64) -> Result<(), String> {
    let ra = self.reduce(b)?;
    emit_jumps(b, format!("{}", ra).as_str(), to, from);
    Ok(())
  }

  fn box_clone(&self) -> Box<dyn Expression> {
    Box::new(self.clone())
  }
}

impl Clone for AccessOp {
  fn clone(&self) -> Self {
    AccessOp {
      array: Box::new(*self.array.clone()),
      index: self.index.box_clone(),
      typ: self.typ.clone(),
    }
  }
}

impl fmt::Display for AccessOp {
  fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
    write!(f, "{} [ {} ]", self.array, self.index)
  }
}

struct RelationOp {
  op: Token,
  left: Box<dyn Expression>,
  right: Box<dyn Expression>,
}

impl RelationOp {
  fn new(op: Token, left: Box<dyn Expression>, right: Box<dyn Expression>) -> Result<RelationOp, String> {
    if left.typ() != right.typ() {
      return Err(String::from("Type error"));
    }
    match left.typ() {
      Type::Array { of: _, length: _ } => return Err(String::from("Type error")),
      _ => ()
    }
    match right.typ() {
      Type::Array { of: _, length: _ } => return Err(String::from("Type error")),
      _ => ()
    }

    Ok(RelationOp { op: op, left: left, right: right })
  }
}

impl Expression for RelationOp {
  fn op(&self) -> Token {
    self.op.clone()
  }

  fn typ(&self) -> Type {
    Type::boolean()
  }

  fn generate(&self, b: &mut String) -> Result<Box<dyn Expression>, String> {
    let f = new_label();
    let a = new_label();
    let tmp = Temp::new(self.typ());
    self.jumps(b, 0, f)?;
    emit(b, format!("{} = true", tmp).as_str());
    emit(b, format!("goto L{}", a).as_str());
    emit_label(b, f);
    emit(b, format!("{} = false", tmp).as_str());
    emit_label(b, a);
    Ok(Box::new(tmp))
  }

  fn reduce(&self, b: &mut String) -> Result<Box<dyn Expression>, String> {
    Ok(self.box_clone())
  }

  fn jumps(&self, b: &mut String, to: i64, from: i64) -> Result<(), String> {
    let lr = self.left.reduce(b)?;
    let rr = self.right.reduce(b)?;
    emit_jumps(b, format!("{} {} {}", lr, self.op, rr).as_str(), to, from);
    Ok(())
  }

  fn box_clone(&self) -> Box<dyn Expression> {
    Box::new(self.clone())
  }
}

impl Clone for RelationOp {
  fn clone(&self) -> Self {
    RelationOp {
      op: self.op.clone(),
      left: self.left.box_clone(),
      right: self.right.box_clone(),
    }
  }
}

impl fmt::Display for RelationOp {
  fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
    write!(f, "{} {} {}", self.left, self.op, self.right)
  }
}

struct NotLogicOp {
  op: Token,
  expr: Box<dyn Expression>,
}

impl NotLogicOp {
  fn new(op: Token, expr: Box<dyn Expression>) -> Result<NotLogicOp, String> {
    if expr.typ() != Type::boolean() {
      return Err(String::from("Type error"));
    }
    if op.tag() != Token::Tok('!' as u8).tag() {
      return Err(String::from("Lexer error"));
    }
    Ok(NotLogicOp { op: op, expr: expr })
  }
}

impl Expression for NotLogicOp {
  fn op(&self) -> Token {
    self.op.clone()
  }

  fn typ(&self) -> Type {
    Type::boolean()
  }

  fn generate(&self, b: &mut String) -> Result<Box<dyn Expression>, String> {
    let from = new_label();
    let a = new_label();
    let tmp = Temp::new(self.typ());
    self.jumps(b, 0, from)?;
    emit(b, format!("{} = true", tmp).as_str());
    emit(b, format!("goto L{}", a).as_str());
    emit_label(b, from);
    emit(b, format!("{} = false", tmp).as_str());
    emit_label(b, a);
    Ok(Box::new(tmp))
  }

  fn reduce(&self, b: &mut String) -> Result<Box<dyn Expression>, String> {
    Ok(self.box_clone())
  }

  fn jumps(&self, b: &mut String, to: i64, from: i64) -> Result<(), String> {
    self.expr.jumps(b, from, to)
  }

  fn box_clone(&self) -> Box<dyn Expression> {
    Box::new(self.clone())
  }

}

impl Clone for NotLogicOp {
  fn clone(&self) -> Self {
    NotLogicOp { op: self.op.clone(), expr: self.expr.box_clone() }
  }
}

impl fmt::Display for NotLogicOp {
  fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
    write!(f, "{} {}", self.op, self.expr)
  }
}

fn check(tleft: &Type, tright: &Type) -> bool {
  let bt = Type::boolean();
  tleft == bt && tright == bt
}

struct OrLogicOp {
  left: Box<dyn Expression>,
  right: Box<dyn Expression>,
}

impl OrLogicOp {
  fn new(left: Box<dyn Expression>, right: Box<dyn Expression>) -> Result<OrLogicOp, String> {
    if !check(&left.typ(), &right.typ()) {
      return Err(String::from("Type Error"))
    }
    Ok(OrLogicOp { left: left, right: right })
  }
}

impl Expression for OrLogicOp {
  fn op(&self) -> Token {
    Token::or_word()
  }

  fn typ(&self) -> Type {
    Type::boolean()
  }

  fn generate(&self, b: &mut String) -> Result<Box<dyn Expression>, String> {
    let from = new_label();
    let a = new_label();
    let tmp = Temp::new(self.typ());
    self.jumps(b, 0, from)?;
    emit(b, format!("{} = true", tmp).as_str());
    emit(b, format!("goto L{}", a).as_str());
    emit_label(b, from);
    emit(b, format!("{} = false", tmp).as_str());
    emit_label(b, a);
    Ok(Box::new(tmp))
  }

  fn reduce(&self, b: &mut String) -> Result<Box<dyn Expression>, String> {
    Ok(self.box_clone())
  }

  fn jumps(&self, b: &mut String, to: i64, from: i64) -> Result<(), String> {
    let mut label = to;
    if to == 0 {
      label = new_label();
    }
    self.left.jumps(b, label, 0)?;
    self.right.jumps(b, to, from)?;
    if to == 0 {
      emit_label(b, label);
    }
    Ok(())
  }

  fn box_clone(&self) -> Box<dyn Expression> {
    Box::new(self.clone())
  }

}

impl fmt::Display for OrLogicOp {
  fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
    write!(f, "{} || {}", self.left, self.right)
  }
}

impl Clone for OrLogicOp {
  fn clone(&self) -> Self {
    OrLogicOp { left: self.left.box_clone(), right: self.right.box_clone() }
  }
}

struct AndLogicOp {
  left: Box<dyn Expression>,
  right: Box<dyn Expression>,
}

impl AndLogicOp {
  fn new(left: Box<dyn Expression>, right: Box<dyn Expression>) -> Result<AndLogicOp, String> {
    if !check(&left.typ(), &right.typ()) {
      return Err(String::from("Type Error"))
    }
    Ok(AndLogicOp { left: left, right: right })
  }
}

impl Expression for AndLogicOp {
  fn op(&self) -> Token {
    Token::and_word()
  }

  fn typ(&self) -> Type {
    Type::boolean()
  }

  fn generate(&self, b: &mut String) -> Result<Box<dyn Expression>, String> {
    let from = new_label();
    let a = new_label();
    let tmp = Temp::new(self.typ());
    self.jumps(b, 0, from)?;
    emit(b, format!("{} = true", tmp).as_str());
    emit(b, format!("goto L{}", a).as_str());
    emit_label(b, from);
    emit(b, format!("{} = false", tmp).as_str());
    emit_label(b, a);
    Ok(Box::new(tmp))
  }

  fn reduce(&self, b: &mut String) -> Result<Box<dyn Expression>, String> {
    Ok(self.box_clone())
  }

  fn jumps(&self, b: &mut String, to: i64, from: i64) -> Result<(), String> {
    let mut label = from;
    if from == 0 {
      label = new_label();
    }
    self.left.jumps(b, 0, label)?;
    self.right.jumps(b, to, from)?;
    if from == 0 {
      emit_label(b, label);
    }
    Ok(())
  }

  fn box_clone(&self) -> Box<dyn Expression> {
    Box::new(self.clone())
  }

}

impl fmt::Display for AndLogicOp {
  fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
    write!(f, "{} && {}", self.left, self.right)
  }
}

impl Clone for AndLogicOp {
  fn clone(&self) -> Self {
    AndLogicOp { left: self.left.box_clone(), right: self.right.box_clone() }
  }
}

#[cfg(test)]
mod test {
use crate::reset_labels;

use super::*;

use lexer::tokens::Token;
use lexer::tokens::Tag;

#[test]
fn expression_tests() {
  let tests: Vec<(Box<dyn Expression>, &str, &str, &str)> = vec![
    (
      Box::new(Identifier::new(Token::Word(String::from("example"), Tag::ID), Type::integer(), 4)),
      "example",
      "",
      ""
    ),
    (
      Box::new(Temp::new(Type::integer())),
      "t1",
      "",
      ""
    ),
    (
      Box::new(ArithmeticOp::new(
        Token::Tok('+' as u8),
        Box::new(Identifier::new(Token::Word(String::from("x"), Tag::ID), Type::integer(), 4)),
        Box::new(Identifier::new(Token::Word(String::from("y"), Tag::ID), Type::integer(), 4)),
      ).unwrap()),
      "x + y",
      "",
      "\tt1 = x + y\n"
    ),
    (
      Box::new(UnaryOp::new(
        Token::Tok('-' as u8),
        Box::new(Identifier::new(Token::Word(String::from("x"), Tag::ID), Type::integer(), 4))
      ).unwrap()),
      "- x",
      "",
      "\tt1 = - x\n"
    ),
    (
      Box::new(AccessOp::new(
        Box::new(Identifier::new(Token::Word(String::from("arr"), Tag::ID), Type::float(), 4)),
        Box::new(Identifier::new(Token::Word(String::from("x"), Tag::ID), Type::integer(), 4)),
        Type::float()
      )),
      "arr [ x ]",
      "",
      "\tt1 = arr [ x ]\n"
    ),
    (
      Box::new(NotLogicOp::new(
        Token::Tok('!' as u8),
        Box::new(Identifier::new(Token::Word(String::from("x"), Tag::ID), Type::boolean(), 4)),
      ).unwrap()),
      "! x",
      "\tif x goto L1\n\tt1 = true\n\tgoto L2\nL1:\tt1 = false\nL2:",
      "",
    ),
    (
      Box::new(OrLogicOp::new(
        Box::new(Identifier::new(Token::Word(String::from("x"), Tag::ID), Type::boolean(), 4)),
        Box::new(Identifier::new(Token::Word(String::from("y"), Tag::ID), Type::boolean(), 4)),
      ).unwrap()),
      "x || y",
      "\tif x goto L3\n\tiffalse y goto L1\nL3:\tt1 = true\n\tgoto L2\nL1:\tt1 = false\nL2:",
      "",
    ),
    (
      Box::new(AndLogicOp::new(
        Box::new(Identifier::new(Token::Word(String::from("x"), Tag::ID), Type::boolean(), 4)),
        Box::new(Identifier::new(Token::Word(String::from("y"), Tag::ID), Type::boolean(), 4)),
      ).unwrap()),
      "x && y",
      "\tiffalse x goto L1\n\tiffalse y goto L1\n\tt1 = true\n\tgoto L2\nL1:\tt1 = false\nL2:",
      "",
    ),
    (
      Box::new(RelationOp::new(
        Token::eq_word(),
        Box::new(Identifier::new(Token::Word(String::from("x"), Tag::ID), Type::boolean(), 4)),
        Box::new(Identifier::new(Token::Word(String::from("y"), Tag::ID), Type::boolean(), 4)),
      ).unwrap()),
      "x == y",
      "\tiffalse x == y goto L1\n\tt1 = true\n\tgoto L2\nL1:\tt1 = false\nL2:",
      "",
    )
  ];

  for tc in tests {
    Temp::reset_counter();
    reset_labels();

    assert_eq!(format!("{}", tc.0), tc.1);
    let mut b = String::new();
    tc.0.generate(&mut b);
    assert_eq!(b, tc.2);

    let mut b = String::new();
    tc.0.reduce(&mut b);
    assert_eq!(b, tc.3);
  }
}
}
