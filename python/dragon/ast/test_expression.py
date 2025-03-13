import pytest

from dragon import ast
from dragon.lexer import tokens
from dragon.ast import expression as expr

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
  (
    expr.Unary(
      tokens.Token(ord('-')),
      expr.Identifier(tokens.Word("x", tokens.ID), tokens.Type.INT, 4),
    ),
    "- x",
    "",
    "\tt1 = - x\n",
  ),
  (
    expr.Access(
      expr.Identifier(tokens.Word("arr", tokens.ID), tokens.Type.FLOAT, 4),
      expr.Identifier(tokens.Word("x", tokens.ID), tokens.Type.INT, 4),
      tokens.Type.FLOAT,
    ),
    "arr [ x ]",
    "",
    "\tt1 = arr [ x ]\n",
  ),
  (
    expr.Not(
      tokens.Token(ord('!')),
      expr.Identifier(tokens.Word("x", tokens.ID), tokens.Type.BOOL, 4),
    ),
    "! x",
    "\tif x goto L1\n\tt1 = true\n\tgoto L2\nL1:\tt1 = false\nL2:",
    "",
  ),
  (
    expr.Or(
      expr.Identifier(tokens.Word("x", tokens.ID), tokens.Type.BOOL, 4),
      expr.Identifier(tokens.Word("y", tokens.ID), tokens.Type.BOOL, 4),
    ),
    "x || y",
    "\tif x goto L3\n\tiffalse y goto L1\nL3:\tt1 = true\n\tgoto L2\nL1:\tt1 = false\nL2:",
		"",
  ),
  (
    expr.And(
      expr.Identifier(tokens.Word("x", tokens.ID), tokens.Type.BOOL, 4),
      expr.Identifier(tokens.Word("y", tokens.ID), tokens.Type.BOOL, 4),
    ),
    "x && y",
    "\tiffalse x goto L1\n\tiffalse y goto L1\n\tt1 = true\n\tgoto L2\nL1:\tt1 = false\nL2:",
    "",
  ),
  (
    expr.RelationOp(
      tokens.Word.EQ,
      expr.Identifier(tokens.Word("x", tokens.ID), tokens.Type.BOOL, 4),
      expr.Identifier(tokens.Word("y", tokens.ID), tokens.Type.BOOL, 4),
    ),
    "x == y",
    "\tiffalse x == y goto L1\n\tt1 = true\n\tgoto L2\nL1:\tt1 = false\nL2:",
    "",
  ),
])
def test_expression(exp: expr.Expression, s: str, gen: str, red: str):
  ast.reset_labels()
  expr.Temp.reset_count()
  assert str(exp) == s
  assert exp.gen()[1] == gen
  assert exp.reduce()[1] == red
