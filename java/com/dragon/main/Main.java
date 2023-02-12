package com.dragon.main;

import java.io.IOException;

import com.dragon.lexer.Lexer;
import com.dragon.parser.Parser;

public class Main {
  public static void main(String[] args) throws IOException {
    var l = new Lexer(System.in);
    var parser = new Parser(l);
    System.out.println(parser.program());
  }
}
