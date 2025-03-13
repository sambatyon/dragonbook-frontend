package com.dragon.ast;

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

  public Expr gen(StringBuilder b) {
    return this;
  }

  public Expr reduce(StringBuilder b) {
    return this;
  }

  public void jumping(StringBuilder b, int to, int from) {
    emitJumps(b, toString(), to, from);
  }

  public void emitJumps(StringBuilder b, String test, int to, int from) {
    if (to != 0 && from != 0) {
      emit(b, "if" + test + " goto L" + to);
      emit(b, "goto L" + from);
    } else if (to != 0) {
      emit(b, "if " + test + " goto L" + to);
    } else if (from != 0) {
      emit(b, "iffalse " + test + " goto L" + from);
    }
  }

  @Override
  public String toString() {
    return op.toString();
  }
}
