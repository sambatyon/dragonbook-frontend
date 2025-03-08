from . import inter

from dragon.lexer import tokens

class Expression(inter.Node):
  op: tokens.Token
  typ: tokens.Type

  def __init__(self, op: tokens.Token, typ: tokens.Type):
    self.op = op
    self.typ = typ

  def gen(self) -> tuple["Expression", str]:
    return (self, "")

  def reduce(self) -> tuple["Expression", str]:
    return (self, "")

  def __str__(self):
    return str(self.op)

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
  _offset: int

  def __init__(self, id: tokens.Word, type: tokens.Type, b: int):
    super().__init__(id, type)
    self._offset = b

  def offset(self) -> int:
    return self._offset
