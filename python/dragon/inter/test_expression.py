import pytest

from dragon import inter
from dragon.lexer import tokens
from dragon.inter import expression as expr

@pytest.mark.parametrize("exp,s,gen,red", [
  (
    expr.Identifier(tokens.Word("example", tokens.ID), tokens.Type.INT, 4),
    "example",
    "",
    ""
  ),
  (
    expr.Temp(tokens.Type.INT),
    "t1",
    "",
    "",
  ),
  (
    expr.Arithmetic(
      tokens.Token(ord("+")),
      expr.Identifier(tokens.Word("x", tokens.ID), tokens.Type.INT, 4),
      expr.Identifier(tokens.Word("y", tokens.ID), tokens.Type.INT, 4),
    ),
    "x + y",
    "",
    "\tt1 = x + y\n",
  ),
])
def test_expression(exp: expr.Expression, s: str, gen: str, red: str):
  inter.reset_labels()
  expr.Temp.reset_count()
  assert str(exp) == s
  assert exp.gen()[1] == gen
  assert exp.reduce()[1] == red
