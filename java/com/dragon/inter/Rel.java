package com.dragon.inter;

import com.dragon.lexer.Token;
import com.dragon.symbols.Array;
import com.dragon.symbols.Type;

public class Rel extends Logical {
  public Rel(Token tok, Expr left, Expr right) {
    super(tok, left, right);
  }

  @Override
  public Type check(Type tLeft, Type tRight) {
    if (tLeft instanceof Array || tRight instanceof Array) {
      return null;
    }
    if (tLeft == tRight) {
      return Type.Bool;
    }
    return null;
  }

  @Override
  public void jumping(StringBuilder b, int to, int from) {
    var lr = left.reduce(b);
    var rr = right.reduce(b);

    var test = lr.toString() + " " + op.toString() + " " + rr.toString();
    emitJumps(b, test, to, from);
  }
}
