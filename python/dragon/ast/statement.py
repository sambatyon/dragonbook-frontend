from typing import Optional, override

from dragon import ast
from dragon.lexer import tokens
from dragon.ast import expression as expr

class Statement(ast.Node):
  _after: int = 0

  def gen(self, begin: int, after: int) -> str:
    return ""

class Assign(Statement):
  __id: expr.Identifier
  __expr: expr.Expression

  def __init__(self, id: expr.Identifier, expr: expr.Expression) -> None:
    if self._check(id.type, expr.type) is None:
      self.error("type error")
    self.__id = id
    self.__expr = expr

  def _check(self, left_t: tokens.Type, right_t: tokens.Type) -> Optional[tokens.Type]:
    if tokens.Type.is_numeric(left_t) and tokens.Type.is_numeric(right_t):
      return right_t
    if left_t == tokens.Type.BOOL and right_t == tokens.Type.BOOL:
      return right_t
    return None

  @override
  def gen(self, begin: int, after: int) -> str:
    ex, exstr = self.__expr.gen()
    return "".join([
      exstr,
      ast.emit(f"{self.__id} = {ex}")
    ])

class AssignArray(Statement):
  __array: expr.Identifier
  __expr: expr.Expression
  __index: expr.Expression

  def __init__(self, acc: expr.Access, exp: expr.Expression) -> None:
    if self._check(acc.type, exp.type) is None:
      self.error("type error")
    self.__array = acc.array
    self.__index = acc.index
    self.__expr = exp

  @override
  def _check(self, left_t: tokens.Type, right_t: tokens.Type) -> Optional[tokens.Type]:
    if isinstance(left_t, tokens.Array) or isinstance(right_t, tokens.Array):
      return None
    if left_t == right_t:
      return right_t
    if tokens.Type.is_numeric(left_t) and tokens.Type.is_numeric(right_t):
      return right_t
    return None

  @override
  def gen(self, begin: int, after: int) -> str:
    idx, idxstr = self.__index.reduce()
    ex, exstr = self.__expr.reduce()
    return "".join([
      idxstr,
      exstr,
      ast.emit(f"{self.__array} [ {idx} ] = {ex}")
    ])

class Sequence(Statement):
  __head: Statement
  __tail: Statement

  def __init__(self, head: Statement, tail: Statement) -> None:
    self.__head = head
    self.__tail = tail

  @override
  def gen(self, begin: int, after: int) -> str:
    if self.__head is None:
      return self.__tail.gen(begin, after)
    if self.__tail is None:
      return self.__head.gen(begin, after)
    label = ast.new_label()
    return "".join([
      self.__head.gen(begin, label),
      ast.emit_label(label),
      self.__tail.gen(label, after)
    ])

class If(Statement):
  __cond: expr.Expression
  __stmt: Statement

  def __init__(self, cond: expr.Expression, stmt: Statement) -> None:
    if cond.type != tokens.Type.BOOL:
      cond.error("boolean required in if")
    self.__cond = cond
    self.__stmt = stmt

  @override
  def gen(self, begin: int, after: int) -> str:
    label: int = ast.new_label()
    return "".join([
      self.__cond.jumping(0, after),
      ast.emit_label(label),
      self.__stmt.gen(label, after)
    ])

class Else(Statement):
  __cond: expr.Expression
  __true_stmt: Statement
  __false_stmt: Statement

  def __init__(self, cond: expr.Expression, true_stmt: Statement, false_stmt: Statement) -> None:
    if cond.type != tokens.Type.BOOL:
      cond.error("boolean required in if")
    self.__cond = cond
    self.__true_stmt = true_stmt
    self.__false_stmt = false_stmt

  @override
  def gen(self, begin: int, after: int) -> str:
    label1: int = ast.new_label()
    label2: int = ast.new_label()
    return "".join([
      self.__cond.jumping(0, label2),
      ast.emit_label(label1),
      self.__true_stmt.gen(label1, after),
      ast.emit(f"goto L{after}"),
      ast.emit_label(label2),
      self.__false_stmt.gen(label2, after)
    ])

class While(Statement):
  __cond: expr.Expression
  __body: Statement

  def __init__(self, cond: Optional[expr.Expression] = None, stmt: Optional[Statement] = None) -> None:
    if cond is None and stmt is not None:
      cond.error("missing condition in while expression")
    if cond is not None and stmt is None:
      cond.error("missing body in while expression")
    if cond is not None and stmt is not None:
      self.init(cond, stmt)

  def init(self, cond: expr.Expression, stmt: Statement) -> None:
    if cond.type != tokens.Type.BOOL:
      cond.error("boolean required in while")
    self.__cond = cond
    self.__body = stmt

  @override
  def gen(self, begin: int, after: int) -> str:
    self._after = after
    label: int = ast.new_label()
    return "".join([
      self.__cond.jumping(0, after),
      ast.emit_label(label),
      self.__body.gen(label, begin),
      ast.emit(f"goto L{begin}")
    ])

class Do(Statement):
  __cond: expr.Expression
  __body: Statement

  def __init__(self, cond: Optional[expr.Expression] = None, stmt: Optional[Statement] = None) -> None:
    if cond is None and stmt is not None:
      cond.error("missing condition in do expression")
    if cond is not None and stmt is None:
      cond.error("missing body in do expression")
    if cond is not None and stmt is not None:
      self.init(cond, stmt)

  def init(self, cond: expr.Expression, stmt: Statement) -> None:
    if cond.type != tokens.Type.BOOL:
      cond.error("boolean required in do")
    self.__cond = cond
    self.__body = stmt

  @override
  def gen(self, begin: int, after: int) -> str:
    self._after = after
    label: int = ast.new_label()
    return "".join([
      self.__body.gen(begin, label),
      ast.emit_label(label),
      self.__cond.jumping(begin, 0),
    ])

class Break(Statement):
  __stmt: Statement

  def __init__(self, enclosing: Optional[Statement]) -> None:
    if enclosing is None:
      self.error("unenclosed break")
    self.__stmt = enclosing

  @override
  def gen(self, begin: int, end: int) -> str:
    return ast.emit(f"goto L{self.__stmt._after}")
