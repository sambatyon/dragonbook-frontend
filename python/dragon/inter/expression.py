from typing import Optional, override

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

  def jumping(self, to: int, fr: int) -> str:
    return self.emit_jumps(f"{self}", to, fr)

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
  def __init__(self, tok: tokens.Token, typ: tokens.Type):
    super().__init__(tok, typ)

  @override
  def reduce(self) -> tuple[Expression, str]:
    x, xstr = self.gen()
    tmp = Temp(self.type)
    return tmp, "".join([xstr, inter.emit(f"{tmp} = {x}")])

class Arithmetic(Operator):
  __left: Expression
  __right: Expression

  def __init__(self, tok: tokens.Token, xl: Expression, xr: Expression):
    typ = tokens.Type.max(xl.type, xr.type)
    if typ is None:
      self.error("type error")
    super().__init__(tok, typ)
    self.__left = xl
    self.__right = xr

  @override
  def gen(self) -> tuple[Expression, str]:
    left, lstr = self.__left.reduce()
    right, rstr = self.__right.reduce()
    return Arithmetic(
      self.op,
      left,
      right
    ), "".join([lstr, rstr])

  @override
  def __str__(self) -> str:
    return f"{self.__left} {self.op} {self.__right}"

class Unary(Operator):
  __expr: Expression

  def __init__(self, tok: tokens.Token, x: Expression):
    typ = tokens.Type.max(tokens.Type.INT, x.type)
    if typ is None:
      self.error("type error")
    super().__init__(tok, typ)
    self.__expr = x

  @override
  def gen(self) -> tuple[Expression, str]:
    ex, exstr = self.__expr.reduce()
    return Unary(self.op, ex), exstr

  @override
  def __str__(self) -> str:
    return f"{self.op} {self.__expr}"

class Access(Operator):
  __array: Identifier
  __index: Expression

  def __init__(self, array: Identifier, index: Expression, typ: tokens.Type):
    super().__init__(tokens.Word.ACCESS, typ)
    self.__array = array
    self.__index = index

  @property
  def array(self) -> Identifier:
    return self.__array

  @property
  def index(self) -> Expression:
    return self.__index

  @override
  def gen(self) -> tuple[Expression, str]:
    idx, idxstr = self.__index.reduce()
    return Access(self.__array, idx, self.type), idxstr

  @override
  def jumping(self, to: int, fr: int) -> str:
    expr, exprstr = self.reduce()
    return "".join([
      exprstr,
      self.emit_jumps(str(expr), to, fr)
    ])

  @override
  def __str__(self) -> str:
    return f"{self.__array} [ {self.__index} ]"

class Logical(Expression):
  _left: Expression
  _right: Expression

  def __init__(self, tok: tokens.Token, xl: Expression, xr: Expression):
    typ = self.check(xl.type, xr.type)
    if typ is None:
      self.error("type error")
    super().__init__(tok, typ)
    self._left = xl
    self._right = xr

  def check(self, left_t: tokens.Type, right_t: tokens.Type) -> Optional[tokens.Type]:
    if left_t == tokens.Type.BOOL and right_t == tokens.Type.BOOL:
      return tokens.Type.BOOL
    else:
      return None

  @override
  def gen(self) -> tuple[Expression, str]:
    f = inter.new_label()
    a = inter.new_label()
    tmp = Temp(self.type)
    return tmp, "".join([
      self.jumping(0, f),
      inter.emit(f"{tmp} = true"),
      inter.emit(f"goto L{a}"),
      inter.emit_label(f),
      inter.emit(f"{tmp} = false"),
      inter.emit_label(a),
    ])

  @override
  def __str__(self) -> str:
    return f"{self._left} {self.op} {self._right}"

class Not(Logical):
  def __init__(self, tok: tokens.Token, exp: Expression):
    super().__init__(tok, exp, exp)

  @override
  def jumping(self, to: int, fr: int) -> str:
    return self._left.jumping(fr, to)

  @override
  def __str__(self) -> str:
    return f"{self.op} {self._left}"

class Or(Logical):
  def __init__(self, xl: Expression, xr: Expression):
    super().__init__(tokens.Word.OR, xl, xr)

  @override
  def jumping(self, to: int, fr: int) -> str:
    label: int = to if to != 0 else inter.new_label()
    return "".join([
      self._left.jumping(label, 0),
      self._right.jumping(to, fr),
      inter.emit_label(label) if to == 0 else ""
    ])

class And(Logical):
  def __init__(self, xl: Expression, xr: Expression):
    super().__init__(tokens.Word.AND, xl, xr)

  @override
  def jumping(self, to: int, fr: int) -> str:
    label: int = fr if fr != 0 else inter.new_label()
    return "".join([
      self._left.jumping(0, label),
      self._right.jumping(to, fr),
      inter.emit_label(label) if fr == 0 else ""
    ])

class RelationOp(Logical):
  def __init__(self, tok: tokens.Token, xl: Expression, xr: Expression):
    super().__init__(tok, xl, xr)

  @override
  def check(self, left_t: tokens.Type, right_t: tokens.Type) -> Optional[tokens.Type]:
    if isinstance(left_t, tokens.Array) or isinstance(right_t, tokens.Array):
      return None
    if left_t == right_t:
      return tokens.Type.BOOL
    return None

  @override
  def jumping(self, to: int, fr: int) -> str:
    lr, lrstr = self._left.reduce()
    rr, rrstr = self._right.reduce()

    test = f"{lr} {self.op} {rr}"
    return "".join([
      lrstr,
      rrstr,
      self.emit_jumps(test, to, fr)
    ])
