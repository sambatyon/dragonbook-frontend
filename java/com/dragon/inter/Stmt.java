package com.dragon.inter;

public class Stmt extends Node {
  public static Stmt Null = new Stmt();
  public static Stmt enclosing = Stmt.Null;

  protected int after = 0;

  public Stmt() {}

  public void gen(int b, int a) {}
}
