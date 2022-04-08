package com.dragon.lexer;

public class Real extends Token {
  private final float value;

  public Real(float v) {
    super(Tag.REAL);
    value = v;
  }

  public float getValue() {
    return value;
  }

  public String toString() {
    return "" + value;
  }
}
