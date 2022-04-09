package com.dragon.inter;

import com.dragon.lexer.Token;

public class And extends Logical {
  public And(Token tok, Expr left, Expr right) {
    super(tok, left, right);
  }

  @Override
  public void jumping(int to, int from) {
    var label = from != 0 ? from : newLabel();
    left.jumping(0, label);
    right.jumping(to, from);
    if (from == 0) {
      emitLabel(label);
    }
  }

}
