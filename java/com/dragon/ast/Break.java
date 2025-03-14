package com.dragon.ast;

public class Break extends Stmt {
  Stmt stmt;

  public Break() {
    if (Stmt.enclosing == null) {
      error("unenclosed break");
    }
    stmt = Stmt.enclosing;
  }

  @Override
  public void gen(StringBuilder b, int begin, int after) {
    emit(b, "goto L" + stmt.afterStmt);
  }
}
