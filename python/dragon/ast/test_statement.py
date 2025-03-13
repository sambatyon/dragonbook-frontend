import pytest

from dragon import inter
from dragon.lexer import tokens
from dragon.inter import expression as expr
from dragon.inter import statement as stmt

@pytest.mark.parametrize("st,want", [
  (
    stmt.Assign(
      expr.Identifier(tokens.Word("x", tokens.ID, ), tokens.Type.INT, 4),
      expr.Constant.integer(42),
    ),
    "\tx = 42\n",
  ),
  (
    stmt.Assign(
      expr.Identifier(tokens.Word("x", tokens.ID, ), tokens.Type.BOOL, 4),
      expr.Constant.true(),
    ),
    "\tx = true\n",
  ),
  (
    stmt.AssignArray(
      expr.Access(
        expr.Identifier(tokens.Word("x", tokens.ID), tokens.Type.INT, 4),
        expr.Constant.integer(0),
        tokens.Type.INT,
      ),
      expr.Constant.integer(42),
    ),
    "\tx [ 0 ] = 42\n",
  ),
  (
    stmt.Sequence(
      stmt.Assign(
        expr.Identifier(tokens.Word("x", tokens.ID), tokens.Type.INT, 4),
        expr.Constant.integer(42),
      ),
      stmt.AssignArray(
        expr.Access(
          expr.Identifier(tokens.Word("x", tokens.ID), tokens.Type.INT, 4),
          expr.Constant.integer(0),
          tokens.Type.INT,
        ),
        expr.Constant.integer(42),
      ),
    ),
    "\tx = 42\nL3:\tx [ 0 ] = 42\n",
  ),
  (
    stmt.If(
      expr.Identifier(tokens.Word("b", tokens.ID), tokens.Type.BOOL, 4),
      stmt.Assign(
        expr.Identifier(tokens.Word("x", tokens.ID), tokens.Type.INT, 4),
        expr.Constant.integer(0),
      ),
    ),
    "\tiffalse b goto L2\nL3:\tx = 0\n",
  ),
  (
    stmt.Else(
      expr.Identifier(tokens.Word("b", tokens.ID), tokens.Type.BOOL, 4),
      stmt.Assign(
        expr.Identifier(tokens.Word("x", tokens.ID), tokens.Type.INT, 4),
        expr.Constant.integer(0),
      ),
      stmt.Assign(
        expr.Identifier(tokens.Word("x", tokens.ID), tokens.Type.INT, 4),
        expr.Constant.integer(42),
      ),
    ),
    "\tiffalse b goto L4\nL3:\tx = 0\n\tgoto L2\nL4:\tx = 42\n",
  ),
  (
    stmt.Do(
      expr.Identifier(tokens.Word("b", tokens.ID), tokens.Type.BOOL, 4),
      stmt.Assign(
        expr.Identifier(tokens.Word("x", tokens.ID), tokens.Type.INT, 4),
        expr.Constant.integer(0),
      ),
    ),
    "\tx = 0\nL3:\tif b goto L1\n",
  ),
])
def test_statement(st: stmt.Statement, want: str) -> None:
  inter.reset_labels()
  expr.Temp.reset_count()
  begin = inter.new_label()
  after = inter.new_label()
  assert st.gen(begin, after) == want
