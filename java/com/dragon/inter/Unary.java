package com.dragon.inter;

import com.dragon.lexer.Token;
import com.dragon.symbols.Type;

public class Unary extends Op {
  private Expr expr;

  public Unary(Token tok, Expr x) { // Handles minus, ! is handled by not.
    super(tok, null);
    expr = x;
    type = Type.max(Type.Int, expr.type);
    if (type == null) {
      error("Type Error");
    }
  }

  @Override
  public Expr gen() {
    return new Unary(op, expr.reduce());
  }

  @Override
  public String toString() {
    return op.toString() + " " + expr.toString();
  }
}
