package com.dragon.lexer;

public class Real extends Token {
  private final double value;

  public Real(double v) {
    super(Tag.REAL);
    value = v;
  }

  public double getValue() {
    return value;
  }

  public String toString() {
    return String.format("%.3f", value);
  }
}
