package com.dragon.inter;

import com.dragon.lexer.Token;
import com.dragon.symbols.Type;

public class Expr extends Node {
  protected Token op;
  protected Type type;

  public Expr(Token tok, Type p) {
    op = tok;
    type = p;
  }

  public Token getOp() {
    return op;
  }

  public Type getType() {
    return type;
  }

  public Expr gen() {
    return this;
  }

  public Expr reduce() {
    return this;
  }

  public void jumping(int to, int from) {
    emitJumps(toString(), to, from);
  }

  public void emitJumps(String test, int to, int from) {
    if (to != 0 && from != 0) {
      emit("if" + test + " goto L" + to);
      emit("goto L" + from);
    } else if (to != 0) {
      emit("if " + test + " goto L" + to);
    } else if (from != 0) {
      emit("iffalse " + test + " goto L" + from);
    }
  }

  @Override
  public String toString() {
    return op.toString();
  }
}
