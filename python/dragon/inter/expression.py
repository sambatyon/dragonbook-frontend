from typing import override

from dragon.lexer import tokens

from . import inter

class Expression(inter.Node):
  __op: tokens.Token
  __typ: tokens.Type

  def __init__(self, op: tokens.Token, typ: tokens.Type):
    self.__op = op
    self.__typ = typ

  @property
  def op(self) -> tokens.Token:
    return self.__op

  @property
  def type(self) -> tokens.Type:
    return self.__typ

  def gen(self) -> tuple["Expression", str]:
    return (self, "")

  def reduce(self) -> tuple["Expression", str]:
    return (self, "")

  def __str__(self):
    return str(self.__op)

  def jumping(self, t: int, f: int) -> str:
    return self.emit_jumps(str(self), t, f)

  def emit_jumps(self, test: str, to: int, fr: int) -> str:
    if to != 0 and fr != 0:
      return ''.join([
        inter.emit(f"if {test} goto L{to}"),
        inter.emit(f"goto L{fr}")
      ])
    elif to != 0:
      return inter.emit(f"if {test} goto L{to}")
    elif fr != 0:
      return inter.emit(f"iffalse {test} goto L{fr}")
    else:
      return ""

class Ident(Expression):
  __offset: int

  def __init__(self, id: tokens.Word, type: tokens.Type, b: int):
    super().__init__(id, type)
    self.__offset = b

  @property
  def offset(self) -> int:
    return self.__offset
