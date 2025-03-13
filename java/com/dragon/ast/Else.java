package com.dragon.ast;

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
  public void gen(StringBuilder b, int begin, int after) {
    var label1 = newLabel();
    var label2 = newLabel();
    condition.jumping(b, 0, label2);
    emitLabel(b, label1);
    trueStmt.gen(b, label1, after);
    emit(b, "goto L" + after);
    emitLabel(b, label2);
    falseStmt.gen(b, label2, after);
  }
}
