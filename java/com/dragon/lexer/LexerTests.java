package com.dragon.lexer;

import static org.junit.Assert.assertEquals;

import java.io.ByteArrayInputStream;
import java.io.IOException;
import java.util.Arrays;
import java.util.List;
import org.junit.Test;

import com.dragon.symbols.Type;

public class LexerTests {
  private class TestCase {
    public TestCase(String source, List<Token> want) {
      this.source = source;
      this.want = want;
    }
    public String source;
    public List<Token> want;
  }

  @Test
  public void lexerTests() throws IOException {
    TestCase[] testCases = {
      new TestCase("&", Arrays.asList(new Token('&'))),
      new TestCase("&&", Arrays.asList(Word.and)),
      new TestCase("|", Arrays.asList(new Token('|'))),
      new TestCase("||", Arrays.asList(Word.or)),
      new TestCase("!", Arrays.asList(new Token('!'))),
      new TestCase("!=", Arrays.asList(Word.ne)),
      new TestCase("<", Arrays.asList(new Token('<'))),
      new TestCase("<=", Arrays.asList(Word.le)),
      new TestCase(">", Arrays.asList(new Token('>'))),
      new TestCase(">=", Arrays.asList(Word.ge)),
      new TestCase("1982", Arrays.asList(new Num(1982))),
      new TestCase("1981.2981", Arrays.asList(new Real(1981.2981))),
      new TestCase("Iden7ifer23", Arrays.asList(new Word("Iden7ifer23", Tag.ID))),
      new TestCase("""
        {
          int i; int j; float v; float[100] a;
          while (true) {
            do i = i + 1; while(a[i] < v);
            do j = j - 1; while(a[j] > v);
            if (i >= j) break;
            int x = a[i];
            a[i] = a[j];
            a[j] = x;
          }
        }
      """,
      Arrays.asList(
        new Token('{'),
        Type.Int, new Word("i", Tag.ID), new Token(';'),
        Type.Int, new Word("j", Tag.ID), new Token(';'),
        Type.Float, new Word("v", Tag.ID), new Token(';'),
        Type.Float, new Token('['), new Num(100), new Token(']'), new Word("a", Tag.ID), new Token(';'),
        new Word("while", Tag.WHILE), new Token('('), Word.True, new Token(')'), new Token('{'),
        new Word("do", Tag.DO), new Word("i", Tag.ID), new Token('='), new Word("i", Tag.ID), new Token('+'), new Num(1), new Token(';'),
        new Word("while", Tag.WHILE), new Token('('), new Word("a", Tag.ID), new Token('['), new Word("i", Tag.ID), new Token(']'), new Token('<'), new Word("v", Tag.ID), new Token(')'), new Token(';'),
        new Word("do", Tag.DO), new Word("j", Tag.ID), new Token('='), new Word("j", Tag.ID), new Token('-'), new Num(1), new Token(';'),
        new Word("while", Tag.WHILE), new Token('('), new Word("a", Tag.ID), new Token('['), new Word("j", Tag.ID), new Token(']'), new Token('>'), new Word("v", Tag.ID), new Token(')'), new Token(';'),
        new Word("if", Tag.IF), new Token('('), new Word("i", Tag.ID), Word.ge, new Word("j", Tag.ID), new Token(')'), new Word("break", Tag.BREAK), new Token(';'),
        Type.Int, new Word("x", Tag.ID), new Token('='), new Word("a", Tag.ID), new Token('['), new Word("i", Tag.ID), new Token(']'), new Token(';'),
        new Word("a", Tag.ID), new Token('['), new Word("i", Tag.ID), new Token(']'), new Token('='), new Word("a", Tag.ID), new Token('['), new Word("j", Tag.ID), new Token(']'), new Token(';'),
        new Word("a", Tag.ID), new Token('['), new Word("j", Tag.ID), new Token(']'), new Token('='), new Word("x", Tag.ID), new Token(';'),
        new Token('}'),
        new Token('}')
      )),
    };

    for (var tc : Arrays.asList(testCases)) {
      var lex = new Lexer(new ByteArrayInputStream(tc.source.getBytes()));
      for (var tk : tc.want) {
        var tok = lex.scan();
        assertEquals(tok.toString(), tk.toString());
        assertEquals(tok.getTag(), tk.getTag());
      }
    }
  }
}
