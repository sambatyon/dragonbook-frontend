use std::fmt;
use std::sync::atomic::{AtomicI32, Ordering};

use lexer::tokens::Token;
use lexer;

use super::{emit, emit_jumps, emit_label, new_label, Type};

pub trait Expression: fmt::Display {
  fn op(&self) -> &Token;
  fn typ(&self) -> &Type;

  // TODO(sambatyon): This should take a label generator
  fn generate(&self, b: &mut String) -> Result<Box<dyn Expression>, String> {
    Ok(self.box_clone())
  }

  fn reduce(&self, b: &mut String) -> Result<Box<dyn Expression>, String> {
    Ok(self.box_clone())
  }

  fn jumps(&self, b: &mut String, to: i64, from: i64) -> Result<(), String> {
    emit_jumps(b, format!("{}", &self).as_str(), to, from);
    Ok(())
  }

  fn box_clone(&self) -> Box<dyn Expression>;
}

#[derive(Clone,Eq)]
pub struct Constant {
  token: Token,
  typ: Type
}

impl Constant {
  pub fn integer(value: i64) -> Constant {
    Constant::new(Token::Integer(value)).unwrap()
  }

  pub fn float(value: f64) -> Constant {
    Constant::new(Token::Real(value)).unwrap()
  }

  pub fn new(tok: Token) -> Result<Constant, String> {
    match tok {
      Token::Integer(_) => Ok(Constant{token: tok, typ: Type::integer().clone()}),
      Token::Real(_) => Ok(Constant{token: tok, typ: Type::float().clone()}),
      t => Err(format!("Invalid parameter: {}", t))
    }
  }

  pub fn true_constant() -> Constant {
    Constant{token: Token::true_token().clone(), typ: Type::boolean().clone()}
  }

  pub fn false_constant() -> Constant {
    Constant{token: Token::false_token().clone(), typ: Type::boolean().clone()}
  }
}

impl Expression for Constant {
  fn op(&self) -> &Token {
    &self.token
  }

  fn typ(&self) -> &Type {
    &self.typ
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

#[derive(Clone)]
pub struct Identifier {
  id: Token,
  typ: Type,
  offset: i32,
}

impl Identifier {
  pub fn new(id: Token, typ: &Type, offset: i32) -> Identifier {
    Identifier { id: id, typ: typ.clone(), offset: offset }
  }

  pub fn new_box(id: Token, typ: &Type, offset: i32) -> Box<Identifier> {
    Box::new(Identifier::new(id, typ, offset))
  }
}

impl Expression for Identifier {
  fn op(&self) -> &Token {
    &self.id
  }

  fn typ(&self) -> &Type {
    &self.typ
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
pub struct Temp {
  op: Token,
  typ: Type,
  num: i32,
}

static temp_counter: AtomicI32 = AtomicI32::new(1);

impl Temp {
  pub fn new(typ: &Type) -> Temp {
    Temp{
      op: Token::temp_word().clone(),
      typ: typ.clone(),
      num: temp_counter.fetch_add(1, Ordering::Relaxed)
    }
  }

  pub fn new_box(typ: &Type) -> Box<Temp> {
    Box::new(Temp::new(typ))
  }

  pub fn reset_counter() {
    temp_counter.store(1, Ordering::Relaxed)
  }
}

impl Expression for Temp {
  fn op(&self) -> &Token {
    &self.op
  }

  fn typ(&self) -> &Type {
    &self.typ
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

pub struct ArithmeticOp {
  op: Token,
  typ: Type,
  left: Box<dyn Expression>,
  right: Box<dyn Expression>,
}

impl ArithmeticOp {
  pub fn new(tok: Token, left: Box<dyn Expression>, right: Box<dyn Expression>) -> Result<ArithmeticOp, String> {
    let typ = match Type::max_type(left.typ(), right.typ()) {
      Some(t) => t,
      None => return Err(String::from("Type error"))
    };
    Ok(ArithmeticOp{ op: tok, typ: typ, left: left, right: right })
  }

  pub fn new_box(tok: Token, left: Box<dyn Expression>, right: Box<dyn Expression>) -> Result<Box<ArithmeticOp>, String> {
    let ao = ArithmeticOp::new(tok, left, right)?;
    Ok(Box::new(ao))
  }
}

impl Expression for ArithmeticOp {
  fn op(&self) -> &Token {
    &self.op
  }

  fn typ(&self) -> &Type {
    &self.typ
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

pub struct UnaryOp {
  op: Token,
  typ: Type,
  rest: Box<dyn Expression>,
}

impl UnaryOp {
  pub fn new(op: Token, rest: Box<dyn Expression>) -> Result<UnaryOp, String> {
    let typ = match Type::max_type(Type::integer(), rest.typ()) {
      Some(typ) => typ,
      _ => return Err(String::from("Type Error"))
    };
    Ok(UnaryOp { op: op, typ: typ, rest: rest })
  }

  pub fn new_box(op: Token, rest: Box<dyn Expression>) -> Result<Box<UnaryOp>, String> {
    let uo = UnaryOp::new(op, rest)?;
    Ok(Box::new(uo))
  }
}

impl Expression for UnaryOp {
  fn op(&self) -> &Token {
    &self.op
  }

  fn typ(&self) -> &Type {
    &self.typ
  }

  fn generate(&self, b: &mut String) -> Result<Box<dyn Expression>, String> {
    let rest = self.rest.reduce(b)?;
    let unary = UnaryOp::new(self.op.clone(), rest)?;
    Ok(Box::new(unary))
  }

  fn reduce(&self, b: &mut String) -> Result<Box<dyn Expression>, String> {
    let x = self.generate(b)?;
    let tmp = Temp::new(self.typ());
    emit(b, format!("{} = {}", tmp, x).as_str());
    Ok(Box::new(tmp))
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

pub struct AccessOp {
  pub array: Box<Identifier>,
  pub index: Box<dyn Expression>,
  typ: Type,
}

impl AccessOp {
  pub fn new(array: Box<Identifier>, index: Box<dyn Expression>, typ: &Type) -> AccessOp {
    AccessOp { array: array, index: index, typ: typ.clone() }
  }

  pub fn new_box(array: Box<Identifier>, index: Box<dyn Expression>, typ: &Type) -> Box<AccessOp> {
    Box::new(AccessOp::new(array, index, typ))
  }
}

impl Expression for AccessOp {
  fn op(&self) -> &Token {
    lexer::tokens::Token::access_word()
  }

  fn typ(&self) -> &Type {
    &self.typ
  }

  fn generate(&self, b: &mut String) -> Result<Box<dyn Expression>, String> {
    let idx = self.index.generate(b)?;
    Ok(Box::new(AccessOp::new(self.array.clone(), idx, &self.typ)))
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

pub struct RelationOp {
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

  fn new_box(op: Token, left: Box<dyn Expression>, right: Box<dyn Expression>) -> Result<Box<RelationOp>, String> {
    let ro = RelationOp::new(op, left, right)?;
    Ok(Box::new(ro))
  }
}

impl Expression for RelationOp {
  fn op(&self) -> &Token {
    &self.op
  }

  fn typ(&self) -> &Type {
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

pub struct NotLogicOp {
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

  fn new_box(op: Token, expr: Box<dyn Expression>) -> Result<Box<NotLogicOp>, String> {
    let nl = NotLogicOp::new(op, expr)?;
    Ok(Box::new(nl))
  }
}

impl Expression for NotLogicOp {
  fn op(&self) -> &Token {
    &self.op
  }

  fn typ(&self) -> &Type {
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

fn check_booleans(tleft: &Type, tright: &Type) -> bool {
  let bt = Type::boolean();
  tleft == bt && tright == bt
}

pub struct OrLogicOp {
  left: Box<dyn Expression>,
  right: Box<dyn Expression>,
}

impl OrLogicOp {
  fn new(left: Box<dyn Expression>, right: Box<dyn Expression>) -> Result<OrLogicOp, String> {
    if !check_booleans(left.typ(), right.typ()) {
      return Err(String::from("Type Error"))
    }
    Ok(OrLogicOp { left: left, right: right })
  }

  fn new_box(left: Box<dyn Expression>, right: Box<dyn Expression>) -> Result<Box<OrLogicOp>, String> {
    let ol = OrLogicOp::new(left, right)?;
    Ok(Box::new(ol))
  }
}

impl Expression for OrLogicOp {
  fn op(&self) -> &Token {
    Token::or_word()
  }

  fn typ(&self) -> &Type {
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

pub struct AndLogicOp {
  left: Box<dyn Expression>,
  right: Box<dyn Expression>,
}

impl AndLogicOp {
  fn new(left: Box<dyn Expression>, right: Box<dyn Expression>) -> Result<AndLogicOp, String> {
    if !check_booleans(left.typ(), right.typ()) {
      return Err(String::from("Type Error"))
    }
    Ok(AndLogicOp { left: left, right: right })
  }

  fn new_box(left: Box<dyn Expression>, right: Box<dyn Expression>) -> Result<Box<AndLogicOp>, String> {
    let al = AndLogicOp::new(left, right)?;
    Ok(Box::new(al))
  }
}

impl Expression for AndLogicOp {
  fn op(&self) -> &Token {
    Token::and_word()
  }

  fn typ(&self) -> &Type {
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

use lexer::tokens::{Tag, Token};

#[test]
fn expression_tests() {
  let tests: Vec<(Box<dyn Expression>, &str, &str, &str)> = vec![
    (
      Identifier::new_box(Token::from_str("example"), Type::integer(), 4),
      "example",
      "",
      ""
    ),
    (
      Temp::new_box(Type::integer()),
      "t1",
      "",
      ""
    ),
    (
      ArithmeticOp::new_box(
        Token::Tok('+' as u8),
        Identifier::new_box(Token::from_str("x"), Type::integer(), 4),
        Identifier::new_box(Token::from_str("y"), Type::integer(), 4),
      ).unwrap(),
      "x + y",
      "",
      "\tt1 = x + y\n"
    ),
    (
      UnaryOp::new_box(
        Token::Tok('-' as u8),
        Identifier::new_box(Token::from_str("x"), Type::integer(), 4)
      ).unwrap(),
      "- x",
      "",
      "\tt1 = - x\n"
    ),
    (
      AccessOp::new_box(
        Identifier::new_box(Token::from_str("arr"), Type::float(), 4),
        Identifier::new_box(Token::from_str("x"), Type::integer(), 4),
        Type::float()
      ),
      "arr [ x ]",
      "",
      "\tt1 = arr [ x ]\n"
    ),
    (
      NotLogicOp::new_box(
        Token::Tok('!' as u8),
        Identifier::new_box(Token::from_str("x"), Type::boolean(), 4),
      ).unwrap(),
      "! x",
      "\tif x goto L1\n\tt1 = true\n\tgoto L2\nL1:\tt1 = false\nL2:",
      "",
    ),
    (
      OrLogicOp::new_box(
        Identifier::new_box(Token::from_str("x"), Type::boolean(), 4),
        Identifier::new_box(Token::from_str("y"), Type::boolean(), 4),
      ).unwrap(),
      "x || y",
      "\tif x goto L3\n\tiffalse y goto L1\nL3:\tt1 = true\n\tgoto L2\nL1:\tt1 = false\nL2:",
      "",
    ),
    (
      AndLogicOp::new_box(
        Identifier::new_box(Token::from_str("x"), Type::boolean(), 4),
        Identifier::new_box(Token::from_str("y"), Type::boolean(), 4),
      ).unwrap(),
      "x && y",
      "\tiffalse x goto L1\n\tiffalse y goto L1\n\tt1 = true\n\tgoto L2\nL1:\tt1 = false\nL2:",
      "",
    ),
    (
      RelationOp::new_box(
        Token::eq_word().clone(),
        Identifier::new_box(Token::from_str("x"), Type::boolean(), 4),
        Identifier::new_box(Token::from_str("y"), Type::boolean(), 4),
      ).unwrap(),
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
