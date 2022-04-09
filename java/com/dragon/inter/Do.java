package com.dragon.inter;

import com.dragon.symbols.Type;

public class Do extends Stmt {
  private Expr condition;
  private Stmt body;

  public Do() {
    init(null, null);
  }

  public Do(Expr cond, Stmt bod) {
    init(cond, bod);
  }

  public void init(Expr cond, Stmt bod) {
    condition = cond;
    body = bod;
    if (condition.getType() != Type.Bool) {
      condition.error("boolean expression required in do");
    }
  }

  @Override
  public void gen(int b, int a) {
    after = a;
    int label = newLabel();
    body.gen(b, label);
    emitLabel(label);
    condition.jumping(b, 0);
  }

}
