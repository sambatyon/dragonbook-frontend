package com.dragon.inter;

import com.dragon.symbols.Type;

public class If extends Stmt {
  private Expr condition;
  private Stmt stmt;

  public If(Expr x, Stmt s) {
    condition = x;
    stmt = s;
    if (condition.getType() != Type.Bool) {
      condition.error("Boolean required in if");
    }
  }

  @Override
  public void gen(int b, int a) {
    var label = newLabel();
    condition.jumping(0, a);
    emitLabel(label);
    stmt.gen(label, a);
  }
}
