use std::collections::HashMap;
use std::convert::Into;
use std::mem::swap;
use lexer::tokens as toks;
use ast::expression as expr;
use ast::statement as stmt;
use expr::Expression;

pub struct Environment {
  table: HashMap<String, expr::Identifier>,
  previous: Box<Option<Environment>>,
}

impl Environment {
  fn empty() -> Box<Environment> {
    Box::new(Environment { table: HashMap::new(), previous: Box::new(None) })
  }

  fn new(prev: Box<Environment>) -> Box<Environment> {
    Box::new(Environment { table: HashMap::new(), previous: Box::new(Some(*prev)) })
  }

  fn pop(&mut self) -> Result<Box<Environment>, String> {
    let mut res = Box::new(None);
    swap(&mut self.previous, &mut res);
    match *res {
      Some(env) => Ok(Box::new(env)),
      None => Err(String::from("Popping empty environment"))
    }
  }

  fn put(&mut self, key: &str, value: expr::Identifier) {
    self.table.insert(key.to_string(), value);
  }

  fn get(&self, key: &str) -> Result<expr::Identifier, String> {
    match self.table.get(key) {
      Some(value) => Ok(value.clone()),
      _ => match self.previous.as_ref() {
        Some(env) => env.get(key),
        _ => Err(format!("Undeclrared identifier {}", key))
      }
    }
  }
}

pub struct Parser<T: std::io::Read> {
  lexer: lexer::Lexer<T>,
  lookahead: toks::Token,
  top: Box<Environment>,
  used: i64,
}

impl<T: std::io::Read> Parser<T> {
  pub fn new(lexer: lexer::Lexer<T>) -> Result<Parser<T>, String> {
    let mut res = Parser {
      lexer: lexer,
      lookahead: toks::Token::Eof,
      top: Environment::empty(),
      used: 0,
    };
    res.next()?;
    Ok(res)
  }

  pub fn program(&mut self, s: &mut String) -> Result<(), String> {
    let mut stm = self.block()?;
    let begin = ast::new_label();
    let after = ast::new_label();
    ast::emit_label(s, begin);
    stm.generate(s, begin, after)?;
    ast::emit_label(s, after);
    Ok(())
  }

  fn next(&mut self) -> Result<(), String> {
    self.lookahead = match self.lexer.scan() {
      Ok(tok) => tok,
      Err(err) => return Err(format!("{} near line {}", err, self.lexer.line))
    };
    Ok(())
  }

  fn match_token<U: Into<u32>>(&mut self, tag: U) -> Result<(), String> {
    if !self.lookahead.match_tag(tag) {
      return Err(format!("Syntax error near line {}", self.lexer.line))
    }
    self.next()
  }

  fn block(&mut self) -> Result<Box<dyn stmt::Statement>, String> {
    self.match_token(b'{')?;

    let mut empty = Environment::empty();
    swap(&mut self.top, &mut empty);
    self.top = Environment::new(empty);

    self.decls()?;
    let stmts = self.stmts()?;
    self.match_token(b'}')?;

    self.top = self.top.pop()?;
    Ok(stmts)
  }

  fn decls(&mut self) -> Result<(), String> {
    while self.lookahead.match_tag(toks::Tag::BASIC) {
      let typ = self.typ()?;
      let tok = self.lookahead.clone();
      self.match_token(toks::Tag::ID)?;
      self.match_token(b';')?;
      let id = expr::Identifier::new(tok, &typ, self.used as i32);
      self.top.put(id.to_string().as_str(), id);
      self.used += typ.width() as i64;
    }
    Ok(())
  }

  fn typ(&mut self) -> Result<ast::Type, String> {
    let typ = ast::Type::new(&self.lookahead)?;
    self.match_token(toks::Tag::BASIC)?;
    if !self.lookahead.match_tag(b'[') {
      return Ok(typ)
    }
    self.dims(typ)
  }

  fn dims(&mut self, typ: ast::Type) -> Result<ast::Type, String> {
    self.match_token(b'[')?;
    let tok = self.lookahead.clone();
    self.match_token(toks::Tag::INTEGER)?;
    let size = match tok {
      toks::Token::Integer(val) => val,
      _ => return Err(format!("Syntax error near line {}", self.lexer.line))
    };
    self.match_token(b']')?;

    let mut of = typ.clone();
    if self.lookahead.match_tag(b'[') {
      of = self.dims(typ)?;
    }
    Ok(ast::Type::array(of, size as u32))
  }

  fn stmts(&mut self) -> Result<Box<dyn stmt::Statement>, String> {
    if self.lookahead.match_tag(b'}') {
      return Ok(stmt::NullStmt::new_box())
    }
    let head = self.stmt()?;
    let tail = self.stmts()?;
    Ok(stmt::StmtSeq::new_box(head, tail))
  }

  fn stmt<'a: 'b, 'b>(&mut self) -> Result<Box<dyn stmt::Statement + 'b>, String> {
    const OPEN_BR: u32 = b'{' as u32;
    const SEMICOLON: u32 = b';' as u32;
    const IF: u32 = toks::Tag::IF as u32;
    const WHILE: u32 = toks::Tag::WHILE as u32;
    const DO: u32 = toks::Tag::DO as u32;
    const BREAK: u32 = toks::Tag::BREAK as u32;

    match self.lookahead.tag() {
      SEMICOLON => {
        self.next()?;
        Ok(stmt::NullStmt::new_box())
      },
      IF => {
        self.match_token(IF)?;
        self.match_token(b'(')?;
        let ex = self.boolean()?;
        self.match_token(b')')?;
        let body = self.stmt()?;
        if !self.lookahead.match_tag(toks::Tag::ELSE) {
          let ifs = stmt::IfStmt::new_box(ex, body)?;
          return Ok(ifs)
        }
        self.match_token(toks::Tag::ELSE)?;
        let els = self.stmt()?;
        let r = stmt::ElseStmt::new_box(ex, body, els)?;
        Ok(r)
      },
      WHILE => {
        self.match_token(WHILE)?;
        self.match_token(b'(')?;

        let ex = self.boolean()?;
        if ex.typ() != ast::Type::boolean() {
          return Err(String::from("Expression in boolean condition is required for while loop."))
        }

        self.match_token(b')')?;
        let body = self.stmt()?;
        let stm = stmt::WhileStmt::new_box(ex, body)?;
        Ok(stm)
      },
      DO => {
        self.match_token(DO)?;
        let body = self.stmt()?;

        self.match_token(WHILE)?;
        self.match_token(b'(')?;
        let ex = self.boolean()?;
        if ex.typ() != ast::Type::boolean() {
          return Err(String::from("Expression in boolean condition is required for while loop."))
        }
        self.match_token(b')')?;
        self.match_token(b';')?;
        let stm = stmt::DoStmt::new_box(ex, body)?;
        Ok(stm)
      },
      BREAK => {
        self.match_token(BREAK)?;
        self.match_token(b';')?;
        let brk = stmt::BreakStmt::new_box();
        Ok(brk)
      },
      OPEN_BR => self.block(),
      _ => self.assign()
    }
  }

  fn assign(&mut self) -> Result<Box<dyn stmt::Statement>, String> {
    let tok = self.lookahead.clone();
    self.match_token(toks::Tag::ID)?;

    let id = self.top.get(tok.to_string().as_str())?;

    if self.lookahead.match_tag(b'=') {
      self.next()?;
      let expr = self.boolean()?;
      let stm = stmt::AssignStmt::new_box(Box::new(id), expr)?;
      self.match_token(b';')?;
      return Ok(stm);
    }

    let access = self.offset(id)?;
    self.match_token(b'=')?;
    let expr = self.boolean()?;
    let stm = stmt::AssingArrayStmt::new_box(access, expr)?;
    self.match_token(b';')?;
    Ok(stm)
  }

  fn boolean(&mut self) -> Result<Box<dyn expr::Expression>, String> {
    let mut ex = self.join()?;
    while self.lookahead.match_tag(toks::Tag::OR) {
      self.next()?;
      let right = self.join()?;
      ex = expr::OrLogicOp::new_box(ex, right)?;
    }
    Ok(ex)
  }

  fn join(&mut self) -> Result<Box<dyn expr::Expression>, String> {
    let mut ex = self.equality()?;
    while self.lookahead.match_tag(toks::Tag::AND) {
      self.next()?;
      let right = self.equality()?;
      ex = expr::AndLogicOp::new_box(ex, right)?;
    }
    Ok(ex)
  }

  fn equality(&mut self) -> Result<Box<dyn expr::Expression>, String> {
    let mut ex = self.relation()?;
    while self.lookahead.match_tag(toks::Tag::EQ) || self.lookahead.match_tag(toks::Tag::NE) {
      let tok = self.lookahead.clone();
      self.next()?;
      let right = self.relation()?;
      ex = expr::RelationOp::new_box(tok, ex, right)?;
    }
    Ok(ex)
  }

  fn relation(&mut self) -> Result<Box<dyn expr::Expression>, String> {
    let ex = self.expr()?;
    let tok = self.lookahead.clone();

    const LT: u32 = b'<' as u32;
    const GT: u32 = b'>' as u32;
    const LE: u32 = toks::Tag::LE as u32;
    const GE: u32 = toks::Tag::GE as u32;

    match tok.tag() {
      LT | GT | LE | GE => {
        self.next()?;
        let right = self.expr()?;
        let rel = expr::RelationOp::new_box(tok, ex, right)?;
        Ok(rel)
      },
      _ => Ok(ex)
    }
  }

  fn expr(&mut self) -> Result<Box<dyn expr::Expression>, String> {
    let mut ex = self.term()?;

    while self.lookahead.match_tag(b'+') || self.lookahead.match_tag(b'-') {
      let tok = self.lookahead.clone();
      self.next()?;
      let right = self.term()?;
      ex = expr::ArithmeticOp::new_box(tok, ex, right)?;
    }
    Ok(ex)
  }

  fn term(&mut self) -> Result<Box<dyn expr::Expression>, String> {
    let mut ex = self.unary()?;
    while self.lookahead.match_tag(b'*') || self.lookahead.match_tag(b'/') {
      let tok = self.lookahead.clone();
      self.next()?;
      let right = self.unary()?;
      ex = expr::ArithmeticOp::new_box(tok, ex, right)?;
    }
    Ok(ex)
  }

  fn unary(&mut self) -> Result<Box<dyn expr::Expression>, String> {
    const MINUS: u32 = b'-' as u32;
    const EXCL: u32 = b'!' as u32;
    match self.lookahead.tag() {
      MINUS => {
        self.next()?;
        let mut ex = self.unary()?;
        ex = expr::UnaryOp::new_box(toks::Token::minus_word().clone(), ex)?;
        Ok(ex)
      }
      EXCL => {
        let tok = self.lookahead.clone();
        self.next()?;
        let mut ex = self.unary()?;
        ex = expr::NotLogicOp::new_box(tok, ex)?;
        Ok(ex)
      }
      _ => self.factor()
    }
  }

  fn factor(&mut self) -> Result<Box<dyn expr::Expression>, String> {
    const OPAREN: u32 = b'(' as u32;
    const INTEGER: u32 = toks::Tag::INTEGER as u32;
    const REAL: u32 = toks::Tag::REAL as u32;
    const TRUE: u32 = toks::Tag::TRUE as u32;
    const FALSE: u32 = toks::Tag::FALSE as u32;
    const ID: u32 = toks::Tag::ID as u32;

    match self.lookahead.tag() {
      OPAREN => {
        self.next()?;
        let ex = self.boolean()?;
        self.match_token(b')')?;
        Ok(ex)
      },
      INTEGER | REAL => {
        let ex = expr::Constant::new_box(self.lookahead.clone())?;
        self.next()?;
        Ok(ex)
      },
      TRUE => {
        let ex = expr::Constant::true_constant().box_clone();
        self.next()?;
        Ok(ex)
      },
      FALSE => {
        let ex = expr::Constant::true_constant().box_clone();
        self.next()?;
        Ok(ex)
      },
      ID => {
        let id = self.top.get(format!("{}", self.lookahead).as_str())?;
        self.next()?;
        if self.lookahead.match_tag(b'[') {
          let ex = self.offset(id)?;
          return Ok(ex)
        }
        Ok(Box::new(id))
      },
      _ => Err(String::from("Syntax Error"))
    }
  }

  fn offset(&mut self, id: expr::Identifier) -> Result<Box<expr::AccessOp>, String> {
    let mut typ = id.typ().clone();

    self.match_token(b'[')?;
    let index = self.boolean()?;
    self.match_token(b']')?;

    match typ {
      ast::Type::Array{of, length: _} => typ = *of.clone(),
      _ => return Err(String::from("String error"))
    };

    let width = Box::new(expr::Constant::integer(typ.width() as i64));
    let t1 = expr::ArithmeticOp::new_box(toks::Token::Tok(b'*'), index, width)?;

    let mut loc = t1;
    while self.lookahead.match_tag(b'[') {
      self.match_token(b'[')?;
      let index = self.boolean()?;
      self.match_token(b']')?;

      match typ {
        ast::Type::Array{of, length: _} => typ = *of.clone(),
        _ => return Err(String::from("String error"))
      };
      let width = Box::new(expr::Constant::integer(typ.width() as i64));
      let t1 = expr::ArithmeticOp::new_box(toks::Token::Tok(b'*'), index, width)?;

      let t2 = expr::ArithmeticOp::new_box(toks::Token::Tok(b'+'), loc, t1)?;
      loc = t2;
    }

    Ok(expr::AccessOp::new_box(Box::new(id), loc, &typ))
  }
}

#[cfg(test)]
mod test {
use super::*;

use std::io::BufReader;
use stringreader::StringReader;

#[test]
fn parser_tests() {
  let tests: Vec<(&str, &str)> = vec![
    ("{}", "L1:L2:"),
    ("{int i;}", "L1:L2:"),
    ("{int i;float f;bool[100] b;}", "L1:L2:"),
    ("{int i; i = 10;}", "L1:\ti = 10\nL2:"),
    ("{int i; i = i + 10;}", "L1:\ti = i + 10\nL2:"),
    (
      "{int i;int[20] arr; i = 10; arr[i] = 10;}",
      r#"L1:	i = 10
L3:	t1 = i * 4
	arr [ t1 ] = 10
L2:"#,
    ),
    (
      "{int i; int j; bool a; i = i + 10; j = 11; a = i == j;}",
      r#"L1:	i = i + 10
L3:	j = 11
L4:	iffalse i == j goto L5
	t1 = true
	goto L6
L5:	t1 = false
L6:	a = t1
L2:"#,
    ),
    (
      "{int i; int j; j = 12; while (i > j) i = i + 1;}",
      r#"L1:	j = 12
L3:	iffalse i > j goto L2
L4:	i = i + 1
	goto L3
L2:"#,
    ),
    (
      "{int i; int j; j = 12; do i = i + 1; while (i > j);}",
      r#"L1:	j = 12
L3:	i = i + 1
L4:	if i > j goto L3
L2:"#,
    ),
    (
      "{ while (true) { break; } }",
      r#"L1:L3:	goto L2
	goto L1
L2:"#,
    ),
    (
      "{int i; int j; i = 10; j = 1; while (j < i) { i = i + 1; break;} }",
      r#"L1:	i = 10
L3:	j = 1
L4:	iffalse j < i goto L2
L5:	i = i + 1
L6:	goto L2
	goto L4
L2:"#,
    ),
    (
      "{int i; int j; while (true) i = i + 1;}",
      r#"L1:L3:	i = i + 1
	goto L1
L2:"#,
    ),
    (
      "{int i; int j; i = 10; j = 1; while (j < i) { i = i + 1; break;} }",
      r#"L1:	i = 10
L3:	j = 1
L4:	iffalse j < i goto L2
L5:	i = i + 1
L6:	goto L2
	goto L4
L2:"#,
    ),
    (
      r#"{
        int i; int j; float v; float x; float[100] a;
        while (true) {
          do i = i + 1; while (a[i] < v);
          do j = j - 1; while (a[j] > v);
          if (i >= j) break;
          x = a[i];
          a[i] = a[j];
          a[j] = x;
        }
      }"#,
      r#"L1:L3:	i = i + 1
L5:	t1 = i * 8
	t2 = a [ t1 ]
	if t2 < v goto L3
L4:	j = j - 1
L7:	t3 = j * 8
	t4 = a [ t3 ]
	if t4 > v goto L4
L6:	iffalse i >= j goto L8
L9:	goto L2
L8:	t5 = i * 8
	x = a [ t5 ]
L10:	t6 = i * 8
	t7 = j * 8
	t8 = a [ t7 ]
	a [ t6 ] = t8
L11:	t9 = j * 8
	a [ t9 ] = x
	goto L1
L2:"#,
    ),
  ];

  for tc in tests {
    ast::reset_labels();
    expr::Temp::reset_counter();

    let lexer = lexer::Lexer::new(
      BufReader::new(StringReader::new(tc.0))
    );
    let mut parser = Parser::new(lexer).expect("Creating parser");

    let mut str = String::new();
    parser.program(&mut str).expect("Parsing program");

    assert_eq!(str, tc.1);
  }
}
}
