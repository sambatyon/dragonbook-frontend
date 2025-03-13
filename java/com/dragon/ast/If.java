package com.dragon.ast;

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
  public void gen(StringBuilder b, int begin, int after) {
    var label = newLabel();
    condition.jumping(b, 0, after);
    emitLabel(b, label);
    stmt.gen(b, label, after);
  }
}
