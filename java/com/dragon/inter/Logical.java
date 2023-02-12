package com.dragon.inter;

import com.dragon.lexer.Token;
import com.dragon.symbols.Type;

public class Logical extends Expr {
  protected Expr left;
  protected Expr right;

  public Logical(Token tok, Expr xl, Expr xr) {
    super(tok, null);
    left = xl;
    right = xr;
    type = check(left.type, right.type);
    if (type == null) {
      error("Type Error");
    }
  }

  public Type check(Type tLeft, Type tRight) {
    if (tLeft == Type.Bool && tRight == Type.Bool) {
      return Type.Bool;
    }
    return null;
  }

  @Override
  public Expr gen(StringBuilder b) {
    var f = newLabel();
    var a = newLabel();
    var tmp = new Temp(type);
    jumping(b, 0, f);
    emit(b, tmp.toString() + " = true");
    emit(b, "goto L" + a);
    emitLabel(b, f);
    emit(b, tmp.toString() + " = false");
    emitLabel(b, a);
    return tmp;
  }

  @Override
  public String toString() {
    return left.toString() + " " + op.toString() + " " + right.toString();
  }
}
