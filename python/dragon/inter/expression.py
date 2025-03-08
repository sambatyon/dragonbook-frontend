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

class Identifier(Expression):
  __offset: int

  def __init__(self, id: tokens.Word, type: tokens.Type, b: int):
    super().__init__(id, type)
    self.__offset = b

  @property
  def offset(self) -> int:
    return self.__offset


class Temp(Expression):
  __count: int = 1
  __number: int

  def __init__(self, t: tokens.Type):
    super().__init__(tokens.Word.TEMP, t)
    self.__number = Temp.__count
    Temp.__count += 1

  @override
  def __str__(self) -> str:
    return f"t{self.__number}"

  @staticmethod
  def reset_count() -> None:
    Temp.__count = 1


class Operator(Expression):
  def __init__(self, tok: tokens.Token, p: tokens.Type):
    super().__init__(tok, p)

  @override
  def reduce(self) -> tuple[Expression, str]:
    x, _ = self.gen()
    tmp = Temp(self.type)
    return tmp, inter.emit(f"{tmp} = {x}")

class Arithmetic(Operator):
  __left: Expression
  __right: Expression

  def __init__(self, tok: tokens.Token, xl: Expression, xr: Expression):
    super().__init__(tok, None)
    self.__left = xl
    self.__right = xr
    typ = tokens.Type.max(xl.type, xr.type)
    if typ is None:
      self.error("type error")

  @override
  def gen(self) -> tuple[Expression, str]:
    return Arithmetic(
      self.op,
      self.__left.reduce()[0],
      self.__right.reduce()[0]
    ), ""

  @override
  def __str__(self) -> str:
    return f"{self.__left} {self.op} {self.__right}"
