package com.dragon.inter;

import com.dragon.lexer.Word;
import com.dragon.symbols.Type;

public class Temp extends Expr {
  private static int count = 0;

  private int number = 0;

  public Temp(Type t) {
    super(Word.temp, t);
    number = ++count;
  }

  @Override
  public String toString() {
    return "t" + number;
  }

  static public void resetTempCount() {
    count = 0;
  }
}
