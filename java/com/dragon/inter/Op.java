package com.dragon.inter;

import com.dragon.lexer.Token;
import com.dragon.symbols.Type;

public class Op extends Expr {
  public Op(Token tok, Type p) {
    super(tok, p);
  }

  @Override
  public Expr reduce() {
    var x = gen();
    var tmp = new Temp(getType());
    emit(tmp.toString() + " = " + x.toString());
    return tmp;
  }
}
