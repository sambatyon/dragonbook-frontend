package com.dragon.inter;

public class Node {
  Node() {}

  protected void error(String s, int line) {
    throw new Error("near line " + line + ": " + s);
  }

  protected void error(String s) {
    throw new Error(s);
  }

  protected static int labels = 0;
  public static int newLabel() {
    return ++labels;
  }

  public static void emitLabel(int i) {
    System.out.print("L" + i + ":");
  }

  public static void emit(String s) {
    System.out.println("\t" + s);
  }
}
