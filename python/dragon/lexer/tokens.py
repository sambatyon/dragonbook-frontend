from __future__ import annotations

from typing import Optional, override

Tag = int

AND: Tag = 256
BASIC: Tag = 257
BREAK: Tag = 258
DO: Tag = 259
ELSE: Tag = 260
EQ: Tag = 261
FALSE: Tag = 262
GE: Tag = 263
ID: Tag = 264
IF: Tag = 265
INDEX: Tag = 266
LE: Tag = 267
MINUS: Tag = 268
NE: Tag = 269
NUM: Tag = 270
OR: Tag = 271
REAL: Tag = 272
TEMP: Tag = 273
TRUE: Tag = 274
WHILE: Tag = 275


class Token:
  __tag: Tag

  def __init__(self, tag: Tag|str):
    if isinstance(tag, str):
      tag = ord(tag)
    self.__tag = tag

  @property
  def tag(self) -> Tag:
    return self.__tag

  @override
  def __str__(self) -> str:
    return str(chr(self.__tag))


class Integer(Token):
  __value: int

  def __init__(self, value: int):
    super().__init__(NUM)
    self.__value = value

  @property
  def value(self) -> int:
    return self.__value

  @override
  def __str__(self) -> str:
    return str(self.__value)


class Real(Token):
  __value: float

  def __init__(self, value: float):
    super().__init__(REAL)
    self.__value = value

  @property
  def value(self) -> float:
    return self.__value

  @override
  def __str__(self) -> str:
    return f"{self.__value:.3f}"


class Word(Token):
  __lexeme: str

  def __init__(self, lexeme: str, tag: Tag):
    super().__init__(tag)
    self.__lexeme = lexeme

  @property
  def lexeme(self) -> str:
    return self.__lexeme

  @override
  def __str__(self) -> str:
    return self.__lexeme


Word.AND = Word('&&', AND)
Word.OR = Word('||', OR)
Word.EQ = Word('==', EQ)
Word.NE = Word('!=', NE)
Word.LE = Word('<=', LE)
Word.GE = Word('>=', GE)
Word.MINUS = Word('minus', MINUS)
Word.TRUE = Word('true', TRUE)
Word.FALSE = Word('false', FALSE)
Word.TEMP = Word('t', TEMP)
Word.ACCESS = Word('[]', INDEX)


class Type(Word):
  @staticmethod
  def is_numeric(p: Type) -> bool:
    return p == Type.INT or p == Type.FLOAT or p == Type.CHAR

  @staticmethod
  def max(left: Type, right: Type) -> Optional[Type]:
    if not Type.is_numeric(left) or not Type.is_numeric(right):
      return None
    if left == Type.FLOAT or right == Type.FLOAT:
      return Type.FLOAT
    if left == Type.INT or right == Type.INT:
      return Type.INT
    return Type.CHAR

  def __init__(self, lexeme: str, tag: Tag, width: int):
    super().__init__(lexeme, tag)
    self.__width = width

  @property
  def width(self) -> int:
    return self.__width

Type.INT = Type("int", BASIC, 4)
Type.FLOAT = Type("float", BASIC, 8)
Type.CHAR = Type("char", BASIC, 1)
Type.BOOL = Type("bool", BASIC, 1)

class Array(Type):
  __of: Type
  __size: int

  def __init__(self, size: int, typ: Type):
    super().__init__("[]", INDEX, size * typ.width)
    self.__size = size
    self.__of = typ

  @property
  def size(self) -> int:
    return self.__size

  @property
  def of(self) -> Type:
    return self.__of

  @override
  def __str__(self) -> str:
    return f"[{self.__size}] {self.__of}"
