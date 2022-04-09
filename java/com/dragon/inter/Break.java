package com.dragon.inter;

public class Break extends Stmt {
  Stmt stmt;

  public Break() {
    if (Stmt.enclosing == null) {
      error("unenclosed break");
    }
    stmt = Stmt.enclosing;
  }

  @Override
  public void gen(int b, int a) {
    emit("goto L" + stmt.after);
  }
}
