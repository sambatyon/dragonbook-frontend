package com.dragon.inter;

import com.dragon.lexer.Token;

public class Not extends Logical {
  public Not(Token tok, Expr expr) {
    super(tok, expr, expr);
  }

  @Override
  public void jumping(StringBuilder b, int to, int from) {
    left.jumping(b, from, to);
  }

  @Override
  public String toString() {
    return op.toString() + " " + left.toString();
  }
}
