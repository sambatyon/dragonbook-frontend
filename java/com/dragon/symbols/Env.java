package com.dragon.symbols;

import java.util.Hashtable;

import com.dragon.inter.Id;
import com.dragon.lexer.Token;

public class Env {
  private Hashtable<Token, Id> table;
  protected Env prev;

  public Env(Env env) {
    table = new Hashtable<Token, Id>();
    prev = env;
  }

  public void put(Token w, Id i) {
    table.put(w, i);
  }

  public Id get(Token w) {
    for (Env e = this; e != null; e = e.prev) {
      Id found = e.table.get(w);
      if (found != null) {
        return found;
      }
    }
    return null;
  }
}
