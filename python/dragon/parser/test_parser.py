import io
import textwrap

import pytest

from dragon import parser
from dragon import lexer
from dragon import ast
from dragon.ast import expression as expr

@pytest.mark.parametrize("source,want", [
  ("{}", "L1:L2:"),
  ("{int i;}", "L1:L2:"),
  ("{int i;float f;bool[100] b;}", "L1:L2:"),
  ("{int i; i = 10;}", "L1:\ti = 10\nL2:"),
  ("{int i; i = i + 10;}", "L1:\ti = i + 10\nL2:"),
  (
    "{int i;int[20] arr; i = 10; arr[i] = 10;}",
    "L1:\ti = 10\nL3:\tt1 = i * 4\n\tarr [ t1 ] = 10\nL2:"
  ),
  (
    "{int i; int j; bool a; i = i + 10; j = 11; a = i == j;}",
    textwrap.dedent("""\
      L1:	i = i + 10
      L3:	j = 11
      L4:	iffalse i == j goto L5
      \tt1 = true
      \tgoto L6
      L5:	t1 = false
      L6:	a = t1
      L2:""")
  ),
  (
    "{int i; int j; j = 12; while (i > j) i = i + 1;}",
    textwrap.dedent("""\
    L1:	j = 12
    L3:	iffalse i > j goto L2
    L4:	i = i + 1
    \tgoto L3
    L2:"""),
  ),
  (
    "{int i; int j; j = 12; do i = i + 1; while (i > j);}",
    textwrap.dedent("""\
    L1:	j = 12
    L3:	i = i + 1
    L4:	if i > j goto L3
    L2:"""),
  ),
  (
    "{int i; int j; while (true) i = i + 1;}",
    textwrap.dedent("""\
    L1:L3:	i = i + 1
    \tgoto L1
    L2:"""),
  ),
  (
    "{ while (true) {break;} }",
    textwrap.dedent("""\
    L1:L3:	goto L2
    \tgoto L1
    L2:"""),
  ),
  (
    "{int i; int j; i = 10; j = 1; while (j < i) { i = i + 1; break;} }",
    textwrap.dedent("""\
    L1:	i = 10
    L3:	j = 1
    L4:	iffalse j < i goto L2
    L5:	i = i + 1
    L6:	goto L2
    \tgoto L4
    L2:"""),
  ),
  (
    textwrap.dedent("""\
    {
      int i; int j; float v; float x; float[100] a;
      while (true) {
        do i = i + 1; while (a[i] < v);
        do j = j - 1; while (a[j] > v);
        if (i >= j) break;
        x = a[i];
        a[i] = a[j];
        a[j] = x;
      }
    }"""),
    textwrap.dedent("""\
    L1:L3:	i = i + 1
    L5:	t1 = i * 8
    \tt2 = a [ t1 ]
    \tif t2 < v goto L3
    L4:	j = j - 1
    L7:	t3 = j * 8
    \tt4 = a [ t3 ]
    \tif t4 > v goto L4
    L6:	iffalse i >= j goto L8
    L9:	goto L2
    L8:	t5 = i * 8
    \tx = a [ t5 ]
    L10:	t6 = i * 8
    \tt7 = j * 8
    \tt8 = a [ t7 ]
    \ta [ t6 ] = t8
    L11:	t9 = j * 8
    \ta [ t9 ] = x
    \tgoto L1
    L2:"""),
  ),
])
def test_parser(source: str, want: str):
  ast.reset_labels()
  expr.Temp.reset_count()
  prs = parser.Parser(lexer.Lexer(io.StringIO(source)))
  assert prs.program() == want
