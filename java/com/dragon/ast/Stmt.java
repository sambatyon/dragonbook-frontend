package com.dragon.ast;

public class Stmt extends Node {
  public static Stmt Null = new Stmt();
  public static Stmt enclosing = Stmt.Null;

  protected int afterStmt = 0;

  public Stmt() {}

  public void gen(StringBuilder b, int begin, int after) {}
}
