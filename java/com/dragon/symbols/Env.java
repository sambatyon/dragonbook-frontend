package com.dragon.symbols;

import java.util.HashMap;

import com.dragon.inter.Id;
import com.dragon.lexer.Token;

public class Env {
  private HashMap<Token, Id> table;
  protected Env prev;

  public Env(Env env) {
    table = new HashMap<Token, Id>();
    prev = env;
  }

  public void put(Token w, Id i) {
    table.put(w, i);
  }

  public Id get(Token w) {
    for (var e = this; e != null; e = e.prev) {
      Id found = e.table.get(w);
      if (found != null) {
        return found;
      }
    }
    return null;
  }
}
