package com.dragon.lexer;

public class Num extends Token {
  private final int value;

  public Num(int v) {
    super(Tag.NUM);
    value = v;
  }

  public int getValue() {
    return value;
  }

  public String toString() {
    return "" + value;
  }
}
