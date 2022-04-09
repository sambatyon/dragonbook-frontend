package com.dragon.inter;

import com.dragon.lexer.Token;

public class Not extends Logical {
  public Not(Token tok, Expr expr) {
    super(tok, expr, expr);
  }

  @Override
  public void jumping(int to, int from) {
    left.jumping(from, to);
  }

  @Override
  public String toString() {
    return op.toString() + " " + left.toString();
  }
}
