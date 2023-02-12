package com.dragon.inter;

import com.dragon.symbols.Type;

public class Do extends Stmt {
  private Expr condition;
  private Stmt body;

  public Do() {}

  public Do(Stmt bod, Expr cond) {
    init(bod, cond);
  }

  public void init(Stmt bod, Expr cond) {
    condition = cond;
    body = bod;
    if (condition.getType() != Type.Bool) {
      condition.error("boolean expression required in do");
    }
  }

  @Override
  public void gen(StringBuilder b, int begin, int after) {
    afterStmt = after;
    int label = newLabel();
    body.gen(b, begin, label);
    emitLabel(b, label);
    condition.jumping(b, begin, 0);
  }

}
