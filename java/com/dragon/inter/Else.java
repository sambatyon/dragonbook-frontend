package com.dragon.inter;

import com.dragon.symbols.Type;

public class Else extends Stmt {
  private Expr condition;
  private Stmt trueStmt;
  private Stmt falseStmt;

  public Else(Expr x, Stmt s1, Stmt s2) {
    condition = x;
    trueStmt = s1;
    falseStmt = s2;
    if (condition.getType() != Type.Bool) {
      condition.error("Boolean required in if");
    }
  }

  @Override
  public void gen(int b, int a) {
    var label1 = newLabel();
    var label2 = newLabel();
    condition.jumping(0, label2);
    emitLabel(label1);
    trueStmt.gen(label1, a);
    emit("goto L" + a);
    emitLabel(label2);
    falseStmt.gen(label2, a);
  }
}
