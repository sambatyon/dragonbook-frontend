use std::fmt;
use std::sync::atomic::{AtomicI32, Ordering};
use lexer::tokens::Token;
use lexer;

use super::Type;
use super::emit_jumps;
use super::emit;

pub trait Expression: fmt::Display {
  fn op(&self) -> Token;
  fn typ(&self) -> Type;
  fn generate(&self, b: &mut String) -> Result<Box<dyn Expression>, String>;
  fn reduce(&self, b: &mut String) -> Result<Box<dyn Expression>, String>;
  fn jumps(&self, b: &mut String, to: i64, from: i64) -> Result<(), String>;
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

static temp_counter: AtomicI32 = AtomicI32::new(0);

impl Temp {
  fn new(typ: Type) -> Temp {
    Temp{op: Token::temp_word(), typ: typ, num: temp_counter.fetch_add(1, Ordering::Relaxed)}
  }

  fn reset_counter() {
    temp_counter.store(0, Ordering::Relaxed)
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
}

impl fmt::Display for ArithmeticOp {
  fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
    write!(f, "{} {} {}", self.left, self.op, self.right)
  }
}
