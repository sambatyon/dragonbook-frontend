use lexer::tokens::{Tag, Token};
use std::fmt;
use std::sync::atomic::{AtomicI64, Ordering};

pub mod expression;
pub mod statement;

static temp_counter: AtomicI64 = AtomicI64::new(1);

fn new_label() -> i64 {
  temp_counter.fetch_add(1, Ordering::Relaxed)
}

fn reset_labels() {
  temp_counter.store(1, Ordering::Relaxed);
}

fn emit_label(s: &mut String, i: i64) {
  s.push_str(format!("L{}:", i).as_str());
}

fn emit(s: &mut String, st: &str) {
  s.push_str(format!("\t{}\n", st).as_str());
}

fn emit_jumps(s: &mut String, test: &str, to: i64, from: i64) {
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
  fn new(tok: Token) -> Result<Type, String> {
    match tok {
      Token::SimpleType(lex, w) =>
        Ok(Type::Simple { lexeme: lex.clone(), width: w }),
      Token::Array(of, len) => {
        let o = Type::new(*of)?;
        Ok(Type::Array { of: Box::new(o), length: len })
      },
      _ => Err(format!("Invalid parameters: {}", tok))
    }
  }

  fn integer() -> Type {
    Self::new(Token::integer()).unwrap()
  }

  fn float() -> Type {
    Self::new(Token::float()).unwrap()
  }

  fn ch() -> Type {
    Self::new(Token::ch()).unwrap()
  }

  fn boolean() -> Type {
    Self::new(Token::boolean()).unwrap()
  }

  fn token(&self) -> Token {
    match &self {
      Type::Simple{lexeme, width} =>
        Token::SimpleType(lexeme.clone(), *width),
      Type::Array{of, length} =>
        Token::Array(Box::new(of.token()), *length)
    }
  }

  fn tag(&self) -> Tag {
    match &self {
      Type::Simple{lexeme, width} => Tag::BASIC,
      Type::Array { of, length } => Tag::INDEX
    }
  }

  fn width(&self) -> u32 {
    match &self {
      Type::Simple{lexeme, width} => *width as u32,
      Type::Array { of, length } => of.width() * length
    }
  }

  fn is_numeric(&self) -> bool {
    match &self {
      Type::Simple{lexeme, width} => match lexeme.as_str() {
        "int" | "float" | "char" => true,
        _ => false
      },
      Type::Array{of, length} => false,
    }
  }

  fn max_type(left: &Type, right: &Type) -> Option<Type> {
    if !left.is_numeric() || !right.is_numeric() {
      return None
    }
    let lf = Type::float();
    if left == lf || right == lf {
      return Some(lf)
    }
    let i = Type::integer();
    if left == i || right == i {
      return Some(i)
    }
    Some(Type::ch())
  }
}

impl fmt::Display for Type {
  fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
    match &self {
      Type::Simple{lexeme, width} => write!(f, "{}", lexeme),
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
