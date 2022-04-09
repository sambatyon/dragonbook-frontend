package com.dragon.lexer;

import java.io.IOException;
import java.util.HashMap;

import com.dragon.symbols.Type;

public class Lexer {
  private int line = 1;
  private char peek = ' ';
  private HashMap<String, Word> words = new HashMap<String, Word>();

  public Lexer() {
    reserve(new Word("if", Tag.IF));
    reserve(new Word("else", Tag.ELSE));
    reserve(new Word("while", Tag.WHILE));
    reserve(new Word("do", Tag.DO));
    reserve(new Word("break", Tag.BREAK));
    reserve(Word.True);
    reserve(Word.False);
    reserve(Type.Int);
    reserve(Type.Float);
    reserve(Type.Char);
    reserve(Type.Bool);
  }

  private void reserve(Word w) {
    words.put(w.getLexeme(), w);
  }

  private void readCh() throws IOException {
    peek = (char)System.in.read();
  }

  private boolean readCh(char c) throws IOException {
    readCh();
    if (peek != c) {
      return false;
    }
    peek = ' ';
    return true;
  }

  public Token scan() throws IOException {
    for (;; readCh()) {
      if (peek == ' ' || peek == '\t' || peek == '\r') {
        continue;
      } else if (peek == '\n') {
        line++;
      } else {
        break;
      }
    }

    switch (peek) {
      case '&': {
        if (readCh('&')) {
          return Word.and;
        }
        return new Token('&');
      }
      case '|': {
        if (readCh('|')) {
          return Word.or;
        }
        return new Token('|');
      }
      case '=': {
        if (readCh('=')) {
          return Word.eq;
        }
        return new Token('=');
      }
      case '!': {
        if (readCh('=')) {
          return Word.ne;
        }
        return new Token('!');
      }
      case '<': {
        if (readCh('=')) {
          return Word.le;
        }
        return new Token('<');
      }
      case '>': {
        if (readCh('=')) {
          return Word.ge;
        }
        return new Token('>');
      }
    }

    if (Character.isDigit(peek)) {
      int v = 0;
      do {
        v = 10*v + Character.digit(peek, 10);
        readCh();
      } while (Character.isDigit(peek));

      if (peek != '.') {
        return new Num(v);
      }

      float x = v;
      float d = 10;
      for (;;) {
        readCh();
        if (!Character.isDigit(peek)) {
          break;
        }
        x += Character.digit(peek, 10) / d;
        d *= 10;
      }
      return new Real(x);
    }

    if (Character.isLetter(peek)) {
      var b =  new StringBuffer();
      do {
        b.append(peek);
        readCh();
      } while (Character.isLetterOrDigit(peek));

      var s = b.toString();
      if (words.containsKey(s)) {
        return words.get(s);
      }

      var w = new Word(s, Tag.ID);
      words.put(s, w);
      return w;
    }

    var tok = new Token(peek);
    peek = ' ';
    return tok;
  }

  public int getLine() {
    return line;
  }
}
