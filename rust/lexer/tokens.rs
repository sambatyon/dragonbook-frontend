use std::fmt;
use once_cell::sync::Lazy;

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
  pub fn from_str(ident: &str) -> Token {
    let tag = match ident {
      "if" => Tag::IF,
      "else" => Tag::ELSE,
      "while" => Tag::WHILE,
      "do" => Tag::DO,
      "break" => Tag::BREAK,
      "true" => Tag::TRUE,
      "false" => Tag::FALSE,
      "int" => return Self::integer().clone(),
      "float" => return Self::float().clone(),
      "char" => return Self::ch().clone(),
      "bool" => return Self::boolean().clone(),
      _ => Tag::ID
    };

    Token::Word(ident.to_string(), tag)
  }

  pub fn integer() -> &'static Token {
    static i: Lazy<Token> = Lazy::new(|| {
      Token::SimpleType(String::from("int"), 4u8)
    });
    &*i
  }

  pub fn float() -> &'static Token {
    static f: Lazy<Token> = Lazy::new(|| {
      Token::SimpleType(String::from("float"), 8u8)
    });
    &*f
  }

  pub fn ch() -> &'static Token {
    static c: Lazy<Token> = Lazy::new(|| {
      Token::SimpleType(String::from("char"), 1u8)
    });
    &*c
  }

  pub fn boolean() -> &'static Token {
    static b: Lazy<Token> = Lazy::new(|| {
      Token::SimpleType(String::from("bool"), 1u8)
    });
    &*b
  }

  pub fn true_token() -> &'static Token {
    static t: Lazy<Token> = Lazy::new(|| {
      Token::Word(String::from("true"), Tag::TRUE)
    });
    &*t
  }

  pub fn false_token() -> &'static Token {
    static f: Lazy<Token> = Lazy::new(|| {
      Token::Word(String::from("false"), Tag::FALSE)
    });
    &*f
  }

  pub fn temp_word() -> &'static Token {
    static w: Lazy<Token> = Lazy::new(|| {
      Token::Word(String::from("t"), Tag::TEMP)
    });
    &*w
  }

  pub fn access_word() -> &'static Token {
    static a: Lazy<Token> = Lazy::new(|| {
      Token::Word(String::from("[]"), Tag::INDEX)
    });
    &*a
  }

  pub fn eq_word() -> &'static Token {
    static e: Lazy<Token> = Lazy::new(|| {
      Token::Word(String::from("=="), Tag::EQ)
    });
    &*e
  }

  pub fn or_word() -> &'static Token {
    static o: Lazy<Token> = Lazy::new(|| {
      Token::Word(String::from("||"), Tag::OR)
    });
    &*o
  }

  pub fn and_word() -> &'static Token {
    static a: Lazy<Token> = Lazy::new(|| {
      Token::Word(String::from("&&"), Tag::AND)
    });
    &*a
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
      Token::Tok(tg) => write!(f, "{}", *tg as char),
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

