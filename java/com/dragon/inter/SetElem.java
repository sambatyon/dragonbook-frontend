package com.dragon.inter;

import com.dragon.symbols.Array;
import com.dragon.symbols.Type;

public class SetElem extends Stmt {
  private Id array;
  private Expr index;
  private Expr expr;

  public SetElem(Access x, Expr y) {
    array = x.getArray();
    index = x.getIndex();
    expr = y;
    if (check(x.getType(), expr.getType()) == null) {
      error("Type error");
    }
  }

  private Type check(Type tLeft, Type tRight) {
    if (tLeft instanceof Array || tRight instanceof Array) {
      return null;
    }
    if (tLeft == tRight) {
      return tRight;
    }
    if (Type.isNumeric(tLeft) && Type.isNumeric(tRight)) {
      return tRight;
    }

    return null;
  }

  @Override
  public void gen(int b, int a) {
    var idx = index.reduce().toString();
    var ex = expr.reduce().toString();
    emit(array.toString() + " [ " + idx + " ] = " + ex);
  }
}
