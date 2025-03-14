package com.dragon.ast;

import com.dragon.symbols.Type;

public class Set extends Stmt {
  private Id id;
  private Expr expr;

  public Set(Id i, Expr x) {
    id = i;
    expr = x;
    if (check(id.getType(), expr.getType()) == null) {
      error("Type Error");
    }
  }

  private Type check(Type tLeft, Type tRight) {
    if (Type.isNumeric(tLeft) && Type.isNumeric(tRight)) {
      return tRight;
    }

    if (tLeft == Type.Bool && tRight == Type.Bool) {
      return tRight;
    }

    return null;
  }

  @Override
  public void gen(StringBuilder b, int begin, int after) {
    emit(b, id.toString() + " = " + expr.gen(b).toString());
  }
}
