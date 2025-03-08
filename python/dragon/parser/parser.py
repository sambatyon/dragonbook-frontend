from typing import Optional

from dragon import lexer as lexer
from dragon import inter
from dragon.lexer import tokens
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

  def get(self, w: tokens.Word) -> expr.Identifier:
    if w in self.__table:
        return self.__table[w]
    elif self.__prev is not None:
        return self.__prev.get(w)
    else:
      raise KeyError(w)


class Parser:
  __lex: lexer.Lexer
  __lookahead: tokens.Token
  __top: Optional[Environment] = None
  __used: int = 0

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
    if self.__lookahead.tag() == t:
      self.__move()
    else:
      self.__error("syntax error")

  def program(self) -> str:
    s: stmt.Statement = self.__block()
    begin: int = inter.new_label()
    after: int = inter.new_label()
    return ''.join([
      inter.emit_label(begin),
      s.gen(begin, after),
      inter.emit_label(after)
    ])

  def __block(self) -> stmt.Statement:
      self.__match("{")
      saved_env: Optional[Environment] = self.__top
      top = Environment(top)
      self.__decls()
      s: stmt.Statement = self.__stmts()
      self.__match("}")
      top = saved_env
      return s

  def __decls(self) -> None:
      while self.__lookahead.tag() == tokens.BASIC:
        p: tokens.Type = self.__type()
        tok: tokens.Word = self.__lookahead
        self.__match(tokens.ID)
        self.__match(";")
        id = expr.Identifier(tok, p, self.__used)
        self.__top.put(tok, id)
        self.__used += p.width

  def __type(self) -> tokens.Type:
    p: tokens.Type = tokens.Type(self.__lookahead)
    self.__match(tokens.BASIC)
    if self.__lookahead.tag() != "[":
      return p
    return self.__dims(p)

  def __dims(self, p: tokens.Type) -> tokens.Type:
      self.__match("[")
