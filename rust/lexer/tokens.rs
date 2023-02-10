use std::fmt;

#[derive(Copy,Clone,Debug,Eq)]
pub enum Tag {
  AND = 256,
  BASIC,
  BREAK,
  DO,
  ELSE,
  EQ,
  FALSE,
  GE,
  ID,
  IF,
  INDEX,
  LE,
  MINUS,
  NE,
  INTEGER,
  OR,
  REAL,
  TEMP,
  TRUE,
  WHILE,
  EOF = std::u32::MAX as isize
}

impl PartialEq for Tag {
  fn eq(&self, other: &Self) -> bool {
    (*self as u32) == (*other as u32)
  }
}

#[derive(Clone, Debug)]
pub enum Token {
  Tok(u8),
  Word(String, Tag),
  And,
  Or,
  Equality,
  Ne,
  Le,
  Ge,
  Integer(i64),
  Real(f64),
  SimpleType(String, u8),
  Array(Box<Token>, u32),
  Eof
}

impl Token {
  pub fn from_string(ident: String) -> Token {
    let tag = match ident.as_str() {
      "if" => Tag::IF,
      "else" => Tag::ELSE,
      "while" => Tag::WHILE,
      "do" => Tag::DO,
      "break" => Tag::BREAK,
      "true" => Tag::TRUE,
      "false" => Tag::FALSE,
      "int" => return Self::integer(),
      "float" => return Self::float(),
      "char" => return Self::ch(),
      "bool" => return Self::boolean(),
      _ => Tag::ID
    };

    Token::Word(ident, tag)
  }

  pub fn integer() -> Token {
    Token::SimpleType(String::from("int"), 4u8)
  }

  pub fn float() -> Token {
    Token::SimpleType(String::from("float"), 8u8)
  }

  pub fn ch() -> Token {
    Token::SimpleType(String::from("char"), 1u8)
  }

  pub fn boolean() -> Token {
    Token::SimpleType(String::from("bool"), 1u8)
  }

  pub fn tru() -> Token {
    Token::Word(String::from("true"), Tag::TRUE)
  }

  pub fn fals() -> Token {
    Token::Word(String::from("false"), Tag::FALSE)
  }

  pub fn tag(&self) -> u32 {
    match self {
      Token::Tok(tg) => *tg as u32,
      Token::Word(_, tg) => *tg as u32,
      Token::And => Tag::AND as u32,
      Token::Or => Tag::OR as u32,
      Token::Equality => Tag::EQ as u32,
      Token::Ne => Tag::NE as u32,
      Token::Le => Tag::LE as u32,
      Token::Ge => Tag::GE as u32,
      Token::Integer(_) => Tag::INTEGER as u32,
      Token::Real(_) => Tag::REAL as u32,
      Token::SimpleType(_, _) => Tag::BASIC as u32,
      Token::Array(_, _) => Tag::INDEX as u32,
      Token::Eof => Tag::EOF as u32
    }
  }
}

impl fmt::Display for Token {
  fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
    match self {
      Token::Tok(tg) => write!(f, "{}", tg),
      Token::Word(lex, _) => write!(f, "{}", lex),
      Token::And => write!(f, "&&"),
      Token::Or => write!(f, "||"),
      Token::Equality => write!(f, "=="),
      Token::Ne => write!(f, "!="),
      Token::Le => write!(f, "<="),
      Token::Ge => write!(f, ">="),
      Token::Integer(i) => write!(f, "{}", i),
      Token::Real(r) => write!(f, "{}", r.to_string()),
      Token::SimpleType(lex, _) => write!(f, "{}", lex),
      Token::Array(typ, len) => write!(f, "[{}]{}", len, *typ),
      Token::Eof => write!(f, "\0")
    }
  }
}

impl PartialEq for Token {
  fn eq(&self, other: &Self) -> bool {
    match self {
      Token::Word(text, tag) => match other {
        Token::Word(otext, otag) => text == otext && tag == otag,
        _ => false
      },
      Token::Integer(value) => match other {
        Token::Integer(ovalue) => value == ovalue,
        _ => false
      },
      Token::Real(_) => match other {
        Token::Real(_) => self.to_string() == other.to_string(),
        _ => false
      },
      Token::SimpleType(text, width) => match other {
        Token::SimpleType(otext, owidth) => text == otext && width == owidth,
        _ => false
      },
      Token::Array(typ, len) => match other {
        Token::Array(oty, olen) => typ.as_ref() == oty.as_ref() && len == olen,
        _ => false
      },
      _ => self.tag() == other.tag()
    }
  }
}

impl Eq for Token {}

