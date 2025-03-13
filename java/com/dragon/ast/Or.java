package com.dragon.ast;

import com.dragon.lexer.Token;

public class Or extends Logical {
  public Or(Token tok, Expr left, Expr right) {
    super(tok, left, right);
  }

  @Override
  public void jumping(StringBuilder b, int to, int from) {
    var label = to != 0 ? to : newLabel();
    left.jumping(b, label, 0);
    right.jumping(b, to, from);
    if (to == 0) {
      emitLabel(b, label);
    }
  }
}
