package com.dragon.ast;

import com.dragon.lexer.Word;
import com.dragon.symbols.Type;

public class Id extends Expr {
  private int offset;

  public Id(Word id, Type p, int b) {
    super(id, p);
    offset = b;
  }

  public int getOffset() {
    return offset;
  }
}
