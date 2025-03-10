import io

import pytest

import dragon.parser as parser
import dragon.lexer as lexer

@pytest.mark.parametrize("source,want", [
])
def test_parser(source: str, want: str):
  prs = parser.Parser(lexer.Lexer(io.StringIO(source)))
