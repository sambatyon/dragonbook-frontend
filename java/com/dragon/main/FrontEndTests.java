package com.dragon.main;

import org.junit.runner.RunWith;
import org.junit.runners.Suite;

import com.dragon.lexer.LexerTests;
import com.dragon.parser.ParserTests;
import com.dragon.inter.ExprTests;
import com.dragon.inter.StmtTests;

@RunWith(Suite.class)
@Suite.SuiteClasses({
  LexerTests.class,
  ExprTests.class,
  StmtTests.class,
  ParserTests.class,
})
public class FrontEndTests {

}
