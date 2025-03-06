from typing import Mapping, List
from dragon.lexer import tokens

import io


class Lexer:
  def __init__(self, source: io.TextIOBase) -> None:
    self.__line: int = 1
    self.__peek: str = ' '
    self.__words: Mapping[str, tokens.Word] = {}
    self.__source: io.TextIOBase = source

    self._reserve(tokens.Word('if', tokens.IF))
    self._reserve(tokens.Word('else', tokens.ELSE))
    self._reserve(tokens.Word('while', tokens.WHILE))
    self._reserve(tokens.Word('do', tokens.DO))
    self._reserve(tokens.Word('break', tokens.BREAK))
    self._reserve(tokens.Word.TRUE)
    self._reserve(tokens.Word.FALSE)
    self._reserve(tokens.Type.INT)
    self._reserve(tokens.Type.FLOAT)
    self._reserve(tokens.Type.CHAR)
    self._reserve(tokens.Type.BOOL)

  def _reserve(self, w: tokens.Word) -> None:
    self.__words[w.lexeme] = w

  def _load_ch(self) -> None:
    self.__peek = self.__source.read(1)

  def _read_ch(self, c: str) -> bool:
    self._load_ch()
    if self.__peek != c:
      return False
    self.__peek = ' '
    return True

  def scan(self) -> tokens.Token:
    while True:
      match self.__peek:
        case ' ' | '\t' | '\r':
          pass
        case '\n':
          self.__line += 1
        case _:
          break
      self._load_ch()

    match self.__peek:
      case '&':
        if self._read_ch('&'):
          return tokens.Word.AND
        return tokens.Token(ord('&'))
      case '|':
        if self._read_ch('|'):
          return tokens.Word.OR
        return tokens.Token(ord('|'))
      case '=':
        if self._read_ch('='):
          return tokens.Word.EQ
        return tokens.Token(ord('='))
      case '!':
        if self._read_ch('='):
          return tokens.Word.NE
        return tokens.Token(ord('!'))
      case '<':
        if self._read_ch('='):
          return tokens.Word.LE
        return tokens.Token(ord('<'))
      case '>':
        if self._read_ch('='):
          return tokens.Word.GE
        return tokens.Token(ord('>'))

    if self.__peek.isdigit():
      value = 0
      while True:
        value *= 10
        value += int(self.__peek)
        self._load_ch()
        if not self.__peek.isdigit():
          break

      if self.__peek != ".":
        return tokens.Integer(value)

      fval = float(value)
      d = 10.0
      while True:
        self._load_ch()
        if not self.__peek.isdigit():
          break
        fval += float(self.__peek) / d
        d *= 10.0
      return tokens.Real(fval)

    if self.__peek.isidentifier():
      chars: List[str] = []
      while True:
        chars.append(self.__peek)
        self._load_ch()
        if not self.__peek.isidentifier() and not self.__peek.isdigit():
          break
      s = ''.join(chars)
      if s in self.__words:
        return self.__words[s]

      w = tokens.Word(s, tokens.ID)
      self.__words[s] = w
      return w

    tok = tokens.Token(ord(self.__peek))
    self.__peek = " "
    return tok

  @property
  def line(self) -> int:
    return self.__line
