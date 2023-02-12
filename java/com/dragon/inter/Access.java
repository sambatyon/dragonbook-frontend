package com.dragon.inter;

import com.dragon.lexer.Tag;
import com.dragon.lexer.Word;
import com.dragon.symbols.Type;

public class Access extends Op {
  private Id array;
  private Expr index;

  public Access(Id a, Expr i, Type p) {
    super(new Word("[]", Tag.INDEX), p);
    array = a;
    index = i;
  }

  public Id getArray() {
    return array;
  }

  public Expr getIndex() {
    return index;
  }

  @Override
  public Expr gen(StringBuilder b) {
    return new Access(array, index.reduce(b), type);
  }

  @Override
  public void jumping(StringBuilder b, int to, int from) {
    emitJumps(b, reduce(b).toString(), to, from);
  }

  @Override
  public String toString() {
    return array.toString() + " [ " + index.toString() + " ]";
  }
}
