package com.dragon.parser;

import java.io.ByteArrayInputStream;
import java.io.IOException;
import java.util.Arrays;

import static org.junit.Assert.assertEquals;
import org.junit.Test;

import com.dragon.ast.Node;
import com.dragon.ast.Temp;
import com.dragon.lexer.Lexer;

public class ParserTests {
  private class TestCase {
    public String source;
    public String gen;

    public TestCase(String source, String gen) {
      this.source = source;
      this.gen = gen;
    }
  }

  @Test
  public void testParser() throws IOException {
    TestCase[] testCases = {
      new TestCase("{}", "L1:L2:"),
      new TestCase("{int i;}", "L1:L2:"),
      new TestCase("{int i;float f;bool[100] b;}", "L1:L2:"),
      new TestCase("{int i; i = 10;}", "L1:\ti = 10\nL2:"),
      new TestCase("{int i; i = i + 10;}", "L1:\ti = i + 10\nL2:"),
      new TestCase(
        "{int i;int[20] arr; i = 10; arr[i] = 10;}",
        """
L1:	i = 10
L3:	t1 = i * 4
	arr [ t1 ] = 10
L2:"""),
      new TestCase(
        "{int i; int j; bool a; i = i + 10; j = 11; a = i == j;}",
        """
L1:	i = i + 10
L3:	j = 11
L4:	iffalse i == j goto L5
	t1 = true
	goto L6
L5:	t1 = false
L6:	a = t1
L2:"""),
      new TestCase(
        "{int i; int j; j = 12; while (i > j) i = i + 1;}",
        """
L1:	j = 12
L3:	iffalse i > j goto L2
L4:	i = i + 1
	goto L3
L2:"""),
      new TestCase(
        "{int i; int j; j = 12; do i = i + 1; while (i > j);}",
        """
L1:	j = 12
L3:	i = i + 1
L4:	if i > j goto L3
L2:"""
    ),
      new TestCase(
        "{ while (true) { break; } }",
        """
L1:L3:	goto L2
	goto L1
L2:"""),
      new TestCase(
        "{int i; int j; i = 10; j = 1; while (j < i) { i = i + 1; break;} }",
        """
L1:	i = 10
L3:	j = 1
L4:	iffalse j < i goto L2
L5:	i = i + 1
L6:	goto L2
	goto L4
L2:"""),
      new TestCase(
        "{int i; int j; while (true) i = i + 1;}",
        """
L1:L3:	i = i + 1
	goto L1
L2:"""),
      new TestCase(
        "{int i; int j; i = 10; j = 1; while (j < i) { i = i + 1; break;} }",
        """
L1:	i = 10
L3:	j = 1
L4:	iffalse j < i goto L2
L5:	i = i + 1
L6:	goto L2
	goto L4
L2:"""),
      new TestCase(
        """
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
        }""",
        """
L1:L3:	i = i + 1
L5:	t1 = i * 8
	t2 = a[ t1 ]
	if t2 < v goto L3
L4:	j = j - 1
L7:	t3 = j * 8
	t4 = a[ t3 ]
	if t4 > v goto L4
L6:	iffalse i >= j goto L8
L9:	goto L2
L8:	t5 = i * 8
	x = a[ t5 ]
L10:	t6 = i * 8
	t7 = j * 8
	t8 = a[ t7 ]
	a [ t6 ] = t8
L11:	t9 = j * 8
	a [ t9 ] = x
	goto L1
L2:""")
    };

    for (var tc : Arrays.asList(testCases)) {
      Node.resetLabel();
      Temp.resetTempCount();

      var lex = new Lexer(new ByteArrayInputStream(tc.source.getBytes()));
      var par = new Parser(lex);

      assertEquals(par.program(), tc.gen);
    }
  }
}
