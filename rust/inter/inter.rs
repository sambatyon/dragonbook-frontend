use std::fmt;
use std::cell::RefCell;

use once_cell::sync::Lazy;

use lexer::tokens::{Tag, Token};

pub mod expression;
pub mod statement;

thread_local! {
static LABEL_COUNTER: RefCell<i64> = RefCell::new(1);
}

pub fn new_label() -> i64 {
  let mut res = 0;
  LABEL_COUNTER.with(|counter| {
    res = *counter.borrow();
    *counter.borrow_mut() = res + 1
  });
  res
}

pub fn reset_labels() {
  LABEL_COUNTER.with(|counter| {
    *counter.borrow_mut() = 1;
  });
}

pub fn emit_label(s: &mut String, i: i64) {
  s.push_str(format!("L{}:", i).as_str());
}

pub fn emit(s: &mut String, st: &str) {
  s.push_str(format!("\t{}\n", st).as_str());
}

pub fn emit_jumps(s: &mut String, test: &str, to: i64, from: i64) {
  if to != 0 && from != 0 {
    emit(s, format!("if {} goto L{}", test, to).as_str());
    emit(s, format!("goto L{}", from).as_str());
  } else if to != 0 {
    emit(s, format!("if {} goto L{}", test, to).as_str());
  } else if from != 0 {
    emit(s, format!("iffalse {} goto L{}", test, from).as_str());
  }
}

#[derive(Debug,Clone)]
pub enum Type {
  Simple{lexeme: String, width: u8},
  Array{of: Box<Type>, length: u32}
}

impl Type {
  pub fn new(tok: &Token) -> Result<Type, String> {
    match tok {
      Token::SimpleType(lex, w) =>
        Ok(Type::Simple { lexeme: lex.clone(), width: *w }),
      Token::Array(of, len) => {
        let o = Type::new(&*of)?;
        Ok(Type::Array { of: Box::new(o), length: *len })
      },
      _ => Err(format!("Invalid parameters: {}", tok))
    }
  }

  pub fn array(of: Type, size: u32) -> Type {
    Type::Array { of: Box::new(of), length: size }
  }

  fn integer() -> &'static Type {
    static TYP: Lazy<Type> = Lazy::new(|| Type::new(Token::integer()).unwrap());
    &*TYP
  }

  fn float() -> &'static Type {
    static TYP: Lazy<Type> = Lazy::new(|| Type::new(Token::float()).unwrap());
    &*TYP
  }

  fn ch() -> &'static Type {
    static TYP: Lazy<Type> = Lazy::new(|| Type::new(Token::ch()).unwrap());
    &*TYP
  }

  fn boolean() -> &'static Type {
    static TYP: Lazy<Type> = Lazy::new(|| Type::new(Token::boolean()).unwrap());
    &*TYP
  }

  pub fn token(&self) -> Token {
    match &self {
      Type::Simple{lexeme, width} =>
        Token::SimpleType(lexeme.clone(), *width),
      Type::Array{of, length} =>
        Token::Array(Box::new(of.token()), *length)
    }
  }

  pub fn tag(&self) -> Tag {
    match &self {
      Type::Simple{lexeme: _, width: _} => Tag::BASIC,
      Type::Array { of: _, length: _ } => Tag::INDEX
    }
  }

  pub fn width(&self) -> u32 {
    match &self {
      Type::Simple{lexeme: _, width} => *width as u32,
      Type::Array { of, length } => of.width() * length
    }
  }

  fn is_numeric(&self) -> bool {
    match &self {
      Type::Simple{lexeme, width: _} => match lexeme.as_str() {
        "int" | "float" | "char" => true,
        _ => false
      },
      _ => false,
    }
  }

  fn max_type(left: &Type, right: &Type) -> Option<Type> {
    if !left.is_numeric() || !right.is_numeric() {
      return None
    }
    if left == Type::float() || right == Type::float() {
      return Some(Type::float().clone())
    }
    if left == Type::integer() || right == Type::integer() {
      return Some(Type::integer().clone())
    }
    Some(Type::ch().clone())
  }
}

impl fmt::Display for Type {
  fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
    match &self {
      Type::Simple{lexeme, width: _} => write!(f, "{}", lexeme),
      Type::Array { of, length } => write!(f, "[{}]{}", length, *of)
    }
  }
}

impl PartialEq for Type {
  fn eq(&self, other: &Self) -> bool {
    match &self {
      Type::Simple{lexeme, width} => match other {
        Type::Simple{lexeme: olex, width: ow} => width == ow && lexeme == olex,
        _ => false
      },
      Type::Array { of, length } => match other {
        Type::Array{of: oof, length: olen} => length == olen && of == oof,
        _ => false
      }
    }
  }
}

impl PartialEq<Type> for &Type {
  fn eq(&self, other: &Type) -> bool {
    *self == other
  }
}

impl Eq for Type {}
