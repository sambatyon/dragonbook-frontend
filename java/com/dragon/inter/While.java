package com.dragon.inter;

import com.dragon.symbols.Type;

public class While extends Stmt {
  private Expr condition;
  private Stmt body;

  public While() {}

  public While(Expr cond, Stmt bod) {
    init(cond, bod);
  }

  public void init(Expr cond, Stmt bod) {
    condition = cond;
    body = bod;
    if (condition.getType() != Type.Bool) {
      condition.error("boolean expression required in while");
    }
  }

  @Override
  public void gen(int b, int a) {
    after = a;
    condition.jumping(0, a);
    int label = newLabel();
    emitLabel(label);
    body.gen(label, b);
    emit("goto L" + b);
  }
}
