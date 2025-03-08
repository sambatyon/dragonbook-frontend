import pytest

from dragon import inter
from dragon.lexer import tokens
from dragon.inter import expression as expr

@pytest.mark.parametrize("expr,s,gen,red", [
  (
    expr.Ident(tokens.Word("example", tokens.ID), tokens.Type.INT, 4),
    "example",
    "",
    ""
  ),
])
def test_expression(expr: expr.Expression, s: str, gen: str, red: str):
  assert str(expr) == s
  assert expr.gen()[1] == gen
  assert expr.reduce()[1] == red
