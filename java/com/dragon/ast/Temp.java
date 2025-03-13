package com.dragon.ast;

import com.dragon.lexer.Word;
import com.dragon.symbols.Type;

public class Temp extends Expr {
  private static final ThreadLocal<Integer> count = ThreadLocal.withInitial(() -> 1);

  private int number = 0;

  public Temp(Type t) {
    super(Word.temp, t);
    number = count.get();
    count.set(number+1);
  }

  @Override
  public String toString() {
    return "t" + number;
  }

  static public void resetTempCount() {
    count.set(1);
  }
}
