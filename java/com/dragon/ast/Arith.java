package com.dragon.ast;

import com.dragon.lexer.Token;
import com.dragon.symbols.Type;

public class Arith extends Op {
  private Expr left;
  private Expr right;

  public Arith(Token tok, Expr xl, Expr xr) {
    super(tok, null);
    left = xl;
    right = xr;
    type = Type.max(left.getType(), right.getType());
    if (type == null) {
      error("type error");
    }
  }

  @Override
  public Expr gen(StringBuilder b) {
    return new Arith(op, left.reduce(b), right.reduce(b));
  }

  @Override
  public String toString() {
    return left.toString() + " " + op.toString() + " " + right.toString();
  }
}
