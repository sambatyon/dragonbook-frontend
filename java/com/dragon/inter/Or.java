package com.dragon.inter;

import com.dragon.lexer.Token;

public class Or extends Logical {
  public Or(Token tok, Expr left, Expr right) {
    super(tok, left, right);
  }

  @Override
  public void jumping(int to, int from) {
    var label = to != 0 ? to : newLabel();
    left.jumping(label, 0);
    right.jumping(to, from);
    if (to == 0) {
      emitLabel(label);
    }
  }
}
