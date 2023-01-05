#[derive(Copy,Clone,Debug)]
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
  Array(Box<Token>, i32),
  Eof
}

impl Token {
  pub fn from_string(ident: String) -> Token {
    let tg = match ident.as_str() {
      "if" => Tag::IF,
      "else" => Tag::ELSE,
      "while" => Tag::WHILE,
      "do" => Tag::DO,
      "break" => Tag::BREAK,
      "true" => Tag::TRUE,
      "false" => Tag::FALSE,
      "int" => return Token::SimpleType(ident, 4),
      "float" => return Token::SimpleType(ident, 8),
      "char" => return Token::SimpleType(ident, 1),
      "bool" => return Token::SimpleType(ident, 1),
      _ => Tag::ID
    };

    Token::Word(ident, tg)
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

  pub fn to_string(&self) -> String {
    let binding: String;
    let s = match self {
      Token::Tok(tg) => {
        binding = (*tg as char).to_string();
        binding.as_str()
      }
      Token::Word(lexeme, _) => lexeme.as_str(),
      Token::And => "&&",
      Token::Or => "||",
      Token::Equality => "==",
      Token::Ne => "!=",
      Token::Le => "<=",
      Token::Ge => ">=",
      Token::Integer(i) => {
        binding = i.to_string();
        binding.as_str()
      },
      Token::Real(r) => {
        binding = r.to_string();
        binding.as_str()
      }
      Token::SimpleType(lexeme, _) => lexeme,
      Token::Array(typ, length) => {
        binding = format!("[{}]{}", length, typ.to_string());
        binding.as_str()
      }
      Token::Eof => "\0"
    };
    s.to_string()
  }
}
