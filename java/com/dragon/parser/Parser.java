package com.dragon.parser;

import java.io.IOException;

import com.dragon.inter.Access;
import com.dragon.inter.And;
import com.dragon.inter.Arith;
import com.dragon.inter.Break;
import com.dragon.inter.Constant;
import com.dragon.inter.Do;
import com.dragon.inter.Else;
import com.dragon.inter.Expr;
import com.dragon.inter.Id;
import com.dragon.inter.If;
import com.dragon.inter.Node;
import com.dragon.inter.Not;
import com.dragon.inter.Or;
import com.dragon.inter.Rel;
import com.dragon.inter.Seq;
import com.dragon.inter.Set;
import com.dragon.inter.SetElem;
import com.dragon.inter.Stmt;
import com.dragon.inter.Unary;
import com.dragon.inter.While;
import com.dragon.lexer.Lexer;
import com.dragon.lexer.Num;
import com.dragon.lexer.Tag;
import com.dragon.lexer.Token;
import com.dragon.lexer.Word;
import com.dragon.symbols.Array;
import com.dragon.symbols.Env;
import com.dragon.symbols.Type;

public class Parser {
  private Lexer lex;
  private Token lookahead;
  private Env top = null;
  private int used = 0;

  public Parser(Lexer l) throws IOException {
    lex = l;
    move();
  }

  private void move() throws IOException {
    lookahead = lex.scan();
  }

  private void error(String s) {
    throw new Error("near line " + lex.getLine() + ": " + s);
  }

  private void match(int t) throws IOException {
    if (lookahead.getTag() == t) {
      move();
    } else {
      error("Syntax Error");
    }
  }

  public String program() throws IOException {
    var buf = new StringBuilder();
    Stmt s = block();
    int begin = Node.newLabel();
    int after = Node.newLabel();
    Node.emitLabel(buf, begin);
    s.gen(buf, begin, after);
    Node.emitLabel(buf, after);
    return buf.toString();
  }

  private Stmt block() throws IOException {
    match('{');
    Env savedEnv = top;
    top = new Env(top);
    decls();
    Stmt s = stmts();
    match('}');
    top = savedEnv;
    return s;
  }

  private void decls() throws IOException {
    while (lookahead.getTag() == Tag.BASIC) {
      Type p = type();
      var tok = lookahead;
      match(Tag.ID);
      match(';');
      var id = new Id((Word)tok, p, used);
      top.put(tok, id);
      used += p.getWidth();
    }
  }

  private Type type() throws IOException {
    var p = (Type)lookahead;
    match(Tag.BASIC);
    if (lookahead.getTag() != '[') {
      return p;
    }

    return dims(p);
  }

  private Type dims(Type p) throws IOException {
    match('[');
    var tok = lookahead;
    match(Tag.NUM);
    match(']');

    if (lookahead.getTag() == '[') {
      p = dims(p);
    }
    return new Array(((Num)tok).getValue(), p);
  }

  private Stmt stmts() throws IOException {
    if (lookahead.getTag() == '}') {
      return Stmt.Null;
    }
    return new Seq(stmt(), stmts());
  }

  private Stmt stmt() throws IOException {
    Expr expr;
    Stmt s1;
    Stmt s2;
    Stmt savedStmt;
    switch(lookahead.getTag()) {
      case ';': {
        move();
        return Stmt.Null;
      }
      case Tag.IF: {
        match(Tag.IF);
        match('(');
        expr = bool();
        match(')');
        s1 = stmt();
        if (lookahead.getTag() != Tag.ELSE) {
          return new If(expr, s1);
        }
        match(Tag.ELSE);
        s2 = stmt();
        return new Else(expr, s1, s2);
      }

      case Tag.WHILE: {
        var whileNode = new While();
        savedStmt = Stmt.enclosing;
        Stmt.enclosing = whileNode;
        match(Tag.WHILE);
        match('(');
        expr = bool();
        match(')');
        s1 = stmt();
        whileNode.init(expr, s1);
        Stmt.enclosing = savedStmt;
        return whileNode;
      }

      case Tag.DO: {
        var doNode = new Do();
        savedStmt = Stmt.enclosing;
        Stmt.enclosing = doNode;
        match(Tag.DO);
        s1 = stmt();
        match(Tag.WHILE);
        match('(');
        expr = bool();
        match(')');
        match(';');
        doNode.init(s1, expr);
        Stmt.enclosing = savedStmt;
        return doNode;
      }

      case Tag.BREAK: {
        match(Tag.BREAK);
        match(';');
        return new Break();
      }

      case '{':
        return block();

      default:
        return assign();
    }
  }

  private Stmt assign() throws IOException {
    Stmt stmt;
    var tok = lookahead;
    match(Tag.ID);
    Id id = top.get(tok);
    if (id == null) {
      error(tok.toString() + " undeclared");
    }
    if (lookahead.getTag() == '=') {
      move();
      stmt = new Set(id, bool());
    } else {
      Access access = offset(id);
      match('=');
      stmt = new SetElem(access, bool());
    }
    match(';');
    return stmt;
  }

  private Expr bool() throws IOException {
    Expr expr = join();
    while (lookahead.getTag() == Tag.OR) {
      var tok = lookahead;
      move();
      expr = new Or(tok, expr, join());
    }
    return expr;
  }

  private Expr join() throws IOException {
    Expr expr = equality();
    while (lookahead.getTag() == Tag.AND) {
      var tok = lookahead;
      move();
      expr = new And(tok, expr, equality());
    }
    return expr;
  }

  private Expr equality() throws IOException {
    Expr expr = rel();
    while (lookahead.getTag() == Tag.EQ || lookahead.getTag() == Tag.NE) {
      var tok = lookahead;
      move();
      expr = new Rel(tok, expr, rel());
    }
    return expr;
  }

  private Expr rel() throws IOException {
    Expr expr = expr();
    switch (lookahead.getTag()) {
      case '<': case Tag.LE: case Tag.GE: case '>':
        var tok = lookahead;
        move();
        return new Rel(tok, expr, expr());
      default:
        return expr;
    }
  }

  private Expr expr() throws IOException {
    Expr expr = term();
    while (lookahead.getTag() == '+' || lookahead.getTag() == '-') {
      var tok = lookahead;
      move();
      expr = new Arith(tok, expr, term());
    }
    return expr;
  }

  private Expr term() throws IOException {
    Expr expr = unary();
    while (lookahead.getTag() == '*' || lookahead.getTag() == '/') {
      var tok = lookahead;
      move();
      expr = new Arith(tok, expr, unary());
    }
    return expr;
  }

  private Expr unary() throws IOException {
    if (lookahead.getTag() == '-') {
      move();
      return new Unary(Word.minus, unary());
    }
    if (lookahead.getTag() == '!') {
      var tok = lookahead;
      move();
      return new Not(tok, unary());
    }
    return factor();
  }

  private Expr factor() throws IOException {
    Expr expr = null;
    switch (lookahead.getTag()) {
      case '(':
        move();
        expr = bool();
        match(')');
        return expr;
      case Tag.NUM:
        expr = new Constant(lookahead, Type.Int);
        move();
        return expr;
      case Tag.REAL:
        expr = new Constant(lookahead, Type.Float);
        move();
        return expr;
      case Tag.TRUE:
        expr = Constant.True;
        move();
        return expr;
      case Tag.FALSE:
        expr = Constant.False;
        move();
        return expr;
      case Tag.ID:
        Id id = top.get(lookahead);
        if (id == null) {
          error(lookahead.toString() + " undeclared");
        }
        move();
        if (lookahead.getTag() != '[') {
          return id;
        }
        return offset(id);
      default:
        error("Syntax Error");
        return expr;
    }
  }

  private Access offset(Id id) throws IOException {
    Type type = id.getType();
    match('[');
    Expr index = bool();
    match(']');
    type = ((Array)type).getOf();
    Expr width = new Constant(type.getWidth());
    Expr t1 = new Arith(new Token('*'), index, width);
    Expr loc = t1;

    while (lookahead.getTag() == '[') {
      match('[');
      index = bool();
      match(']');
      type = ((Array)type).getOf();
      width = new Constant(type.getWidth());
      t1 = new Arith(new Token('*'), index, width);
      Expr t2 = new Arith(new Token('+'), loc, t1);
      loc = t2;
    }
    return new Access(id, loc, type);
  }
}
