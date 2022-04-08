package com.dragon.symbols;

import com.dragon.lexer.Tag;

public class Array extends Type {
  private Type of;
  private int size = 1;

  public Array(int sz, Type p) {
    super("[]", Tag.INDEX, sz * p.getWidth());

    size = sz;
    of = p;
  }

  public Type getOf() {
    return of;
  }

  public int size() {
    return size;
  }

  public String toString() {
    return "[" + size + "]" + of.toString();
  }
}
