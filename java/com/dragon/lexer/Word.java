package com.dragon.lexer;

public class Word extends Token {
  public static final Word and = new Word("&&", Tag.AND);
  public static final Word or = new Word("||", Tag.OR);
  public static final Word eq = new Word("==", Tag.EQ);
  public static final Word ne = new Word("!=", Tag.NE);
  public static final Word le = new Word("<=", Tag.LE);
  public static final Word ge = new Word(">=", Tag.GE);
  public static final Word minus = new Word("minus", Tag.MINUS);
  public static final Word True = new Word("true", Tag.TRUE);
  public static final Word False = new Word("false", Tag.FALSE);
  public static final Word temp = new Word("t", Tag.TEMP);

  private String lexeme = "";

  public Word(String s, int tag) {
    super(tag);
    lexeme = s;
  }

  public String getLexeme() {
    return lexeme;
  }

  public String toString() {
    return lexeme;
  }
}
