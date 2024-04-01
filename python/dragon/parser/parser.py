import dragon.lexer as lexer
from dragon.lexer import tokens

class Parser:
  def __init__(self, lex: lexer.Lexer) -> None:
    self.__lex = lex
    self.__lookahead: tokens.Token
