package com.dragon.lexer;

public class Token {
  private final int tag;

  public Token(int t) {
    tag = t;
  }

  public int getTag() {
    return tag;
  }

  public String toString() { return "" + (char)tag; }
}
