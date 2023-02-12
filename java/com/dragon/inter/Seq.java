package com.dragon.inter;

public class Seq extends Stmt {
  Stmt head;
  Stmt tail;

  public Seq(Stmt h, Stmt t) {
    head = h;
    tail = t;
  }

  @Override
  public void gen(StringBuilder b, int begin, int after) {
    if (head == Stmt.Null) {
      tail.gen(b, begin, after);
    } else if (tail == Stmt.Null) {
      head.gen(b, begin, after);
    } else {
      int label = newLabel();
      head.gen(b, begin, label);
      emitLabel(b, label);
      tail.gen(b, label, after);
    }
  }
}
