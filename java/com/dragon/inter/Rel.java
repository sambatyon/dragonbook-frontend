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
  public void jumping(int to, int from) {
    var a = left.reduce();
    var b = right.reduce();

    var test = a.toString() + " " + op.toString() + " " + b.hashCode();
    emitJumps(test, to, from);
  }
}
