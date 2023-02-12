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
  public void gen(StringBuilder b, int begin, int after) {
    afterStmt = after;
    condition.jumping(b, 0, after);
    int label = newLabel();
    emitLabel(b, label);
    body.gen(b, label, begin);
    emit(b, "goto L" + begin);
  }
}
