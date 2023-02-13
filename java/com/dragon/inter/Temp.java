package com.dragon.inter;

import java.util.concurrent.atomic.AtomicInteger;

import com.dragon.lexer.Word;
import com.dragon.symbols.Type;

public class Temp extends Expr {
  private static final AtomicInteger count = new AtomicInteger(0);

  private int number = 0;

  public Temp(Type t) {
    super(Word.temp, t);
    number = count.addAndGet(1);
    var next = number+1;
    count.compareAndSet(number, next);
  }

  @Override
  public String toString() {
    return "t" + number;
  }

  static public void resetTempCount() {
    count.getAndSet(0);
  }
}
