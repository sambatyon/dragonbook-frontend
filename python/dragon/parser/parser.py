from typing import Optional

from dragon import lexer
from dragon.lexer import tokens
from dragon import inter
from dragon.inter import expression as expr
from dragon.inter import statement as stmt


class Environment:
  __table: dict[tokens.Token, expr.Identifier] = {}
  __prev: "Optional[Environment]"

  def __init__(self, prev: "Optional[Environment]") -> None:
    self.__prev = prev

  @property
  def prev(self) -> "Optional[Environment]":
    return self.__prev

  def put(self, w: tokens.Word, i: expr.Identifier) -> None:
    self.__table[w] = i

  def get(self, w: tokens.Word) -> Optional[expr.Identifier]:
    if w in self.__table:
        return self.__table[w]
    elif self.__prev is not None:
        return self.__prev.get(w)
    else:
      return None


class Parser:
  __lex: lexer.Lexer
  __lookahead: tokens.Token
  __top: Optional[Environment] = None
  __used: int = 0

  __enclosing: Optional[stmt.Statement] = None

  def __init__(self, lex: lexer.Lexer) -> None:
    self.__lex = lex
    self.__move()

  def __move(self) -> None:
    self.__lookahead = self.__lex.scan()

  def __error(self, msg: str) -> None:
    raise Exception(f"near line {self.__lex.line}: {msg}")

  def __match(self, t: int|str) -> None:
    if isinstance(t, str):
      t = ord(t)
    if self.__lookahead.tag == t:
      self.__move()
    else:
      self.__error("syntax error")

  def program(self) -> str:
    s: Optional[stmt.Statement] = self.__block()
    begin: int = inter.new_label()
    after: int = inter.new_label()
    return ''.join([
      inter.emit_label(begin),
      s.gen(begin, after) if s is not None else "",
      inter.emit_label(after)
    ])

  def __block(self) -> Optional[stmt.Statement]:
      self.__match("{")
      saved_env: Optional[Environment] = self.__top
      self.__top = Environment(self.__top)
      self.__decls()
      s: Optional[stmt.Statement] = self.__stmts()
      self.__match("}")
      self.__top = saved_env
      return s

  def __decls(self) -> None:
      while self.__lookahead.tag == tokens.BASIC:
        p: tokens.Type = self.__type()
        tok: tokens.Word = self.__lookahead
        self.__match(tokens.ID)
        self.__match(";")
        id = expr.Identifier(tok, p, self.__used)
        self.__top.put(tok, id)
        self.__used += p.width

  def __type(self) -> tokens.Type:
    p: tokens.Type = self.__lookahead
    self.__match(tokens.BASIC)
    if self.__lookahead.tag != ord("["):
      return p
    return self.__dims(p)

  def __dims(self, p: tokens.Type) -> tokens.Type:
      self.__match("[")
      tok: tokens.Token = self.__lookahead
      self.__match(tokens.NUM)
      self.__match("]")

      if self.__lookahead.tag == ord("["):
        p = self.__dims(p)
      return tokens.Array(tok.value, p)

  def __stmts(self) -> Optional[stmt.Statement]:
    if self.__lookahead.tag == ord("}"):
      return None
    head = self.__stmt()
    tail = self.__stmts()
    if head is None:
      return tail
    if tail is None:
      return head
    return stmt.Sequence(head, tail)

  __SEMICOLON = ord(";")
  __OPEN_BRACE = ord("{")

  def __stmt(self) -> Optional[stmt.Statement]:
    match self.__lookahead.tag:
      case Parser.__SEMICOLON:
        self.__move()
        return None
      case tokens.IF:
        self.__match(tokens.IF)
        self.__match('(')
        x: expr.Expression = self.__bool()
        self.__match(')')
        s1: Optional[stmt.Statement] = self.__stmt()
        if s1 is None:
          s1 = stmt.Statement()
        if self.__lookahead.tag != tokens.ELSE:
          return stmt.If(x, s1)
        self.__match(tokens.ELSE)
        s2: Optional[stmt.Statement] = self.__stmt()
        if s2 is None:
          s2 = stmt.Statement()
        return stmt.Else(x, s1, s2)
      case tokens.WHILE:
        w = stmt.While()
        saved = self.__enclosing
        self.__enclosing = w
        self.__match(tokens.WHILE)
        self.__match('(')
        x: expr.Expression = self.__bool()
        self.__match(')')
        s1: Optional[stmt.Statement] = self.__stmt()
        if s1 is None:
          s1 = stmt.Statement()
        w.init(x, s1)
        self.__enclosing = saved
        return w
      case tokens.DO:
        do = stmt.Do()
        saved = self.__enclosing
        self.__enclosing = do
        self.__match(tokens.DO)
        s1: Optional[stmt.Statement] = self.__stmt()
        if s1 is None:
          s1 = stmt.Statement()
        self.__match(tokens.WHILE)
        self.__match('(')
        x: expr.Expression = self.__bool()
        self.__match(')')
        self.__match(';')
        do.init(x, s1)
        self.__enclosing = saved
        return do
      case tokens.BREAK:
        self.__match(tokens.BREAK)
        self.__match(';')
        return stmt.Break(self.__enclosing)
      case Parser.__OPEN_BRACE:
        return self.__block()
      case _:
        return self.__assign()

  def __assign(self) -> stmt.Statement:
    tok: tokens.Token = self.__lookahead
    self.__match(tokens.ID)
    id: expr.Identifier = self.__top.get(tok)
    if id is None:
      self.__error(f"{tok.lexeme} undeclared")
    if self.__lookahead.tag == ord('='):
      self.__move()
      s: stmt.Statement = stmt.Assign(id, self.__bool())
    else:
      acc: expr.Access = self.__offset(id)
      self.__match(ord('='))
      s = stmt.AssignArray(acc, self.__bool())
    self.__match(';')
    return s

  def __bool(self) -> expr.Expression:
    x: expr.Expression = self.__join()
    while self.__lookahead.tag == tokens.OR:
      tok: tokens.Token = self.__lookahead
      self.__move()
      x = expr.Or(x, self.__join())
    return x

  def __join(self) -> expr.Expression:
    x: expr.Expression = self.__equality()
    while self.__lookahead.tag == tokens.AND:
      tok: tokens.Token = self.__lookahead
      self.__move()
      x = expr.And(x, self.__equality())
    return x

  def __equality(self) -> expr.Expression:
    x: expr.Expression = self.__relation()
    while self.__lookahead.tag in [tokens.EQ, tokens.NE]:
      tok: tokens.Token = self.__lookahead
      self.__move()
      x = expr.RelationOp(tok, x, self.__relation())
    return x

  def __relation(self) -> expr.Expression:
    x: expr.Expression = self.__expr()
    if self.__lookahead.tag in [tokens.LE, tokens.GE, ord("<"), ord('>')]:
      tok: tokens.Token = self.__lookahead
      self.__move()
      return expr.RelationOp(tok, x, self.__expr())
    else:
      return x

  def __expr(self) -> expr.Expression:
    x: expr.Expression = self.__term()
    while self.__lookahead.tag in [ord('+'), ord('-')]:
      tok: tokens.Token = self.__lookahead
      self.__move()
      x = expr.Arithmetic(tok, x, self.__term())
    return x

  def __term(self) -> expr.Expression:
    x: expr.Expression = self.__unary()
    while self.__lookahead.tag in [ord('*'), ord('/')]:
      tok: tokens.Token = self.__lookahead
      self.__move()
      x = expr.Arithmetic(tok, x, self.__unary())
    return x

  def __unary(self) -> expr.Expression:
    if self.__lookahead.tag == ord('-'):
      self.__move()
      return expr.Unary(tokens.Word.MINUS, self.__unary())
    elif self.__lookahead.tag == ord('!'):
      tok: tokens.Token = self.__lookahead
      self.__move()
      return expr.Not(tok, self.__unary())
    else:
      return self.__factor()

  def __factor(self) -> expr.Expression:
    match self.__lookahead.tag:
      case '(':
        self.__move()
        x = self.__bool()
        self.__match(')')
        return x
      case tokens.NUM:
        x = expr.Constant.integer(self.__lookahead.value)
        self.__move()
        return x
      case tokens.REAL:
        x = expr.Constant.real(self.__lookahead.value)
        self.__move()
        return x
      case tokens.TRUE:
        x = expr.Constant.true()
        self.__move()
        return x
      case tokens.FALSE:
        x = expr.Constant.false()
        self.__move()
        return x
      case tokens.ID:
        id: expr.Identifier = self.__top.get(self.__lookahead)
        if id is None:
          self.__error(f"{self.__lookahead.lexeme} undeclared")
        self.__move()
        if self.__lookahead.tag != ord('['):
          return id
        return self.__offset(id)
      case _:
        self.__error("syntax error")

  def __offset(self, id: expr.Identifier) -> expr.Access:
    typ: tokens.Type = id.type
    self.__match('[')
    index: expr.Expression = self.__bool()
    self.__match(']')
    typ = typ.of
    width: int = expr.Constant.integer(typ.width)
    t1: expr.Expression = expr.Arithmetic(tokens.Token('*'), index, width)
    loc = t1
    while self.__lookahead.tag == ord('['):
      self.__match('[')
      index = self.__bool()
      self.__match(']')
      typ = typ.of
      width = expr.Constant.integer(typ.width)
      t1 = expr.Arithmetic(tokens.Token('*'), index, width)
      t2 = expr.Arithmetic(tokens.Token('+'), loc, t1)
      loc = t2
    return expr.Access(id, loc, typ)
