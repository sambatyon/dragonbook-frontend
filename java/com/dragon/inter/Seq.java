package com.dragon.inter;

public class Seq extends Stmt {
  Stmt head;
  Stmt tail;

  public Seq(Stmt h, Stmt t) {
    head = h;
    tail = t;
  }

  @Override
  public void gen(int b, int a) {
    if (head == Stmt.Null) {
      tail.gen(b, a);
    } else if (tail == Stmt.Null) {
      head.gen(b, a);
    } else {
      int label = newLabel();
      head.gen(b, label);
      emitLabel(label);
      tail.gen(label, a);
    }
  }
}
