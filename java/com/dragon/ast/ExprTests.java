package com.dragon.ast;

import static org.junit.Assert.assertEquals;

import java.util.Arrays;

import com.dragon.lexer.Tag;
import com.dragon.lexer.Token;
import com.dragon.lexer.Word;
import com.dragon.symbols.Type;

import org.junit.Test;

public class ExprTests {
  private class TestCase {
    public Expr expr;
    public String str;
    public String gen;
    public String red;

    public TestCase(Expr expr, String str, String gen, String red) {
      this.expr = expr;
      this.str = str;
      this.gen = gen;
      this.red = red;
    }
  }

  @Test
  public void exprTests() {
    TestCase[] testCases = {
      new TestCase(
        new Id(new Word("example", Tag.ID), Type.Int, 4),
        "example",
        "",
        ""
      ),
      new TestCase(
        new Temp(Type.Int),
        "t1",
        "",
        ""
      ),
      new TestCase(
        new Arith(
          new Token('+'),
          new Id(new Word("x", Tag.ID), Type.Int, 4),
          new Id(new Word("y", Tag.ID), Type.Int, 4)
        ),
        "x + y",
        "",
        "\tt1 = x + y\n"
      ),
      new TestCase(
        new Unary(
          new Token('-'),
          new Id(new Word("x", Tag.ID), Type.Int, 4)
        ),
        "- x",
        "",
        "\tt1 = - x\n"
      ),
      new TestCase(
        new Access(
          new Id(new Word("arr", Tag.ID), Type.Float, 4),
          new Id(new Word("x", Tag.ID), Type.Int, 4),
          Type.Float
        ),
        "arr[ x ]",
        "",
        "\tt1 = arr[ x ]\n"
      ),
      new TestCase(
        new Or(
          Word.or,
          new Id(new Word("x", Tag.ID), Type.Bool, 4),
          new Id(new Word("y", Tag.ID), Type.Bool, 4)
        ),
        "x || y",
        "\tif x goto L3\n\tiffalse y goto L1\nL3:\tt1 = true\n\tgoto L2\nL1:\tt1 = false\nL2:",
        ""
      ),
      new TestCase(
        new And(
          Word.and,
          new Id(new Word("x", Tag.ID), Type.Bool, 4),
          new Id(new Word("y", Tag.ID), Type.Bool, 4)
        ),
        "x && y",
        "\tiffalse x goto L1\n\tiffalse y goto L1\n\tt1 = true\n\tgoto L2\nL1:\tt1 = false\nL2:",
        ""
      ),
      new TestCase(
        new Rel(
          Word.eq,
          new Id(new Word("x", Tag.ID), Type.Bool, 4),
          new Id(new Word("y", Tag.ID), Type.Bool, 4)
        ),
        "x == y",
        "\tiffalse x == y goto L1\n\tt1 = true\n\tgoto L2\nL1:\tt1 = false\nL2:",
        ""
      ),
    };

    for (var tc : Arrays.asList(testCases)) {
      Node.resetLabel();
      Temp.resetTempCount();

      assertEquals(tc.str, tc.expr.toString());

      var builder = new StringBuilder();
      tc.expr.gen(builder);
      assertEquals(tc.gen, builder.toString());

      builder.setLength(0);
      tc.expr.reduce(builder);
      assertEquals(tc.red, builder.toString());
    }
  }
}
