package com.dragon.ast;

import static org.junit.Assert.assertEquals;

import java.util.Arrays;

import org.junit.Test;

import com.dragon.lexer.Tag;
import com.dragon.lexer.Word;
import com.dragon.symbols.Type;

public class StmtTests {
  private class TestCase {
    public Stmt stmt;
    public String gen;

    public TestCase(Stmt stmt, String gen) {
      this.stmt = stmt;
      this.gen = gen;
    }
  }

  @Test
  public void stmtTests() {
    TestCase[] testCases = {
      new TestCase(
        new Set(
          new Id(new Word("x", Tag.ID), Type.Int, 4),
          new Constant(42)
        ),
        "\tx = 42\n"
      ),
      new TestCase(
        new SetElem(
          new Access(
            new Id(new Word("arr", Tag.ID), Type.Float, 4),
            new Id(new Word("x", Tag.ID), Type.Int, 4),
            Type.Float),
          new Constant(42.0)
        ),
        // TODO(sambatyon): Formatting of floats is using PC locale.
        "\tarr [ x ] = 42,000\n"
      ),
      new TestCase(
        new Seq(
          new Set(
            new Id(new Word("x", Tag.ID), Type.Int, 4),
            new Constant(42)
          ),
          new SetElem(
            new Access(
              new Id(new Word("arr", Tag.ID), Type.Float, 4),
              new Id(new Word("x", Tag.ID), Type.Int, 4),
              Type.Float),
            new Constant(42.0)
          )
        ),
        "\tx = 42\nL3:\tarr [ x ] = 42,000\n"
      ),
      new TestCase(
        new If(
          new Id(new Word("b", Tag.ID), Type.Bool, 4),
          new Set(
            new Id(new Word("x", Tag.ID), Type.Int, 4),
            new Constant(0)
          )
        ),
        "\tiffalse b goto L2\nL3:\tx = 0\n"
      ),
      new TestCase(
        new Else(
          new Id(new Word("b", Tag.ID), Type.Bool, 4),
          new Set(
            new Id(new Word("x", Tag.ID), Type.Int, 4),
            new Constant(0)
          ),
          new Set(
            new Id(new Word("x", Tag.ID), Type.Int, 4),
            new Constant(42)
          )
        ),
        "\tiffalse b goto L4\nL3:\tx = 0\n\tgoto L2\nL4:\tx = 42\n"
      ),
      new TestCase(
        new While(
          new Id(new Word("b", Tag.ID), Type.Bool, 4),
          new Set(
            new Id(new Word("x", Tag.ID), Type.Int, 4),
            new Constant(0)
          )
        ),
        "\tiffalse b goto L2\nL3:\tx = 0\n\tgoto L1\n"
      ),
      new TestCase(
        new Do(
          new Set(
            new Id(new Word("x", Tag.ID), Type.Int, 4),
            new Constant(0)
          ),
          new Id(new Word("b", Tag.ID), Type.Bool, 4)
        ),
        "\tx = 0\nL3:\tif b goto L1\n"
      ),
    };

    for (var tc : Arrays.asList(testCases)) {
      Node.resetLabel();
      Temp.resetTempCount();

      var begin = Node.newLabel();
      var after = Node.newLabel();
      var builder = new StringBuilder();

      tc.stmt.gen(builder, begin, after);
      assertEquals(builder.toString(), tc.gen);
    }
  }
}
