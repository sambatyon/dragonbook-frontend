package com.dragon.ast;

public class Node {
  Node() {}

  protected void error(String s, int line) {
    throw new Error("near line " + line + ": " + s);
  }

  protected void error(String s) {
    throw new Error(s);
  }

  private static final ThreadLocal<Integer> labels = ThreadLocal.withInitial(() -> 1);
  public static int newLabel() {
    var lbl = labels.get();
    labels.set(lbl+1);
    return lbl;
  }
  public static void resetLabel() {
    labels.set(1);
  }

  public static void emitLabel(StringBuilder b, int i) {
    b.append("L" + i + ":");
  }

  public static void emit(StringBuilder b, String s) {
    b.append("\t" + s + "\n");
  }
}
