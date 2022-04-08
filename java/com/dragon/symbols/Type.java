package com.dragon.symbols;

import com.dragon.lexer.Tag;
import com.dragon.lexer.Word;

public class Type extends Word {
  public static final Type Int = new Type("int", Tag.BASIC, 4);
  public static final Type Float = new Type("float", Tag.BASIC, 8);
  public static final Type Char = new Type("char", Tag.BASIC, 1);
  public static final Type Bool = new Type("bool", Tag.BASIC, 1);

  public static boolean isNumeric(Type p) {
    return p == Type.Char || p == Type.Int || p == Type.Float;
  }

  public static Type max(Type left, Type right) {
    if (!isNumeric(left) || !isNumeric(right)) {
      return null;
    }
    if (left == Type.Float || right == Type.Float) {
      return Type.Float;
    }
    if (left == Type.Int || right == Type.Int) {
      return Type.Int;
    }

    return Type.Char;
  }

  private int width = 0;

  public Type(String s, int tag, int w) {
    super(s, tag);
    width = w;
  }

  public int getWidth() {
    return width;
  }
}
