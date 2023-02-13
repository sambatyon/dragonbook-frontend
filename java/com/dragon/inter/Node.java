package com.dragon.inter;

import java.util.concurrent.atomic.AtomicInteger;

public class Node {
  Node() {}

  protected void error(String s, int line) {
    throw new Error("near line " + line + ": " + s);
  }

  protected void error(String s) {
    throw new Error(s);
  }

  private static final AtomicInteger labels = new AtomicInteger(0);
  public static int newLabel() {
    return labels.addAndGet(1);
  }
  public static void resetLabel() {
    labels.getAndSet(0);
  }

  public static void emitLabel(StringBuilder b, int i) {
    b.append("L" + i + ":");
  }

  public static void emit(StringBuilder b, String s) {
    b.append("\t" + s);
  }
}
