#include <gtest/gtest.h>

#include "cpp/lexer/lexer.hpp"
#include "cpp/lexer/numeric.hpp"
#include "cpp/lexer/token.hpp"
#include "cpp/lexer/type.hpp"
#include "cpp/lexer/word.hpp"

#include <sstream>
#include <string>
#include <vector>

using lexer::Token;
using lexer::Word;
using symbols::Type;

struct LexerTestCase {
  std::string source;
  std::vector<std::shared_ptr<Token>> want;
};

TEST(TestLexer, LexerTests) {
  std::vector<LexerTestCase> test_cases{
    LexerTestCase{"&", std::vector<std::shared_ptr<Token>>{
      std::make_shared<Token>('&')
    }},
    LexerTestCase{"&&", std::vector<std::shared_ptr<Token>>{
      Word::and_word
    }},
    LexerTestCase{"|", std::vector<std::shared_ptr<Token>>{
      std::make_shared<Token>('|')
    }},
    LexerTestCase{"||", std::vector<std::shared_ptr<Token>>{
      Word::or_word
    }},
    LexerTestCase{"!", std::vector<std::shared_ptr<Token>>{
      std::make_shared<Token>('!')
    }},
    LexerTestCase{"!=", std::vector<std::shared_ptr<Token>>{
      Word::not_equal
    }},
    LexerTestCase{"<", std::vector<std::shared_ptr<Token>>{
      std::make_shared<Token>('<')
    }},
    LexerTestCase{"<=", std::vector<std::shared_ptr<Token>>{
      Word::less_equal
    }},
    LexerTestCase{">", std::vector<std::shared_ptr<Token>>{
      std::make_shared<Token>('>')
    }},
    LexerTestCase{">=", std::vector<std::shared_ptr<Token>>{
      Word::greater_equal
    }},
    LexerTestCase{"1982", std::vector<std::shared_ptr<Token>>{
      std::make_shared<lexer::Number>(1982)
    }},
    LexerTestCase{"1982.2981", std::vector<std::shared_ptr<Token>>{
      std::make_shared<lexer::Real>(1982.2981)
    }},
    LexerTestCase{"Iden7ifer23", std::vector<std::shared_ptr<Token>>{
      Word::create("Iden7ifer23", Token::kIdentifier)
    }},
    LexerTestCase{
      R"COD(
      {
        int i; int j; float v; float[100] a;
        while (true) {
          do i = i + 1; while(a[i] < v);
          do j = j - 1; while(a[j] > v);
          if (i >= j) break;
          int x = a[i];
          a[i] = a[j];
          a[j] = x;
        }
      }
      )COD",
      std::vector<std::shared_ptr<Token>>{
        std::make_shared<Token>('{'),
        Type::integer, Word::create("i", Token::kIdentifier), std::make_shared<Token>(';'),
        Type::integer, Word::create("j", Token::kIdentifier), std::make_shared<Token>(';'),
        Type::real, Word::create("v", Token::kIdentifier), std::make_shared<Token>(';'),
        Type::real, std::make_shared<Token>('['), std::make_shared<lexer::Number>(100), std::make_shared<Token>(']'), Word::create("a", Token::kIdentifier), std::make_shared<Token>(';'),
        Word::create("while", Token::kWhile), std::make_shared<Token>('('), Word::true_word, std::make_shared<Token>(')'), std::make_shared<Token>('{'),
        Word::create("do", Token::kDo), Word::create("i", Token::kIdentifier), std::make_shared<Token>('='), Word::create("i", Token::kIdentifier), std::make_shared<Token>('+'), std::make_shared<lexer::Number>(1), std::make_shared<Token>(';'),
        Word::create("while", Token::kWhile), std::make_shared<Token>('('), Word::create("a", Token::kIdentifier), std::make_shared<Token>('['), Word::create("i", Token::kIdentifier), std::make_shared<Token>(']'), std::make_shared<Token>('<'), Word::create("v", Token::kIdentifier), std::make_shared<Token>(')'), std::make_shared<Token>(';'),
        Word::create("do", Token::kDo), Word::create("j", Token::kIdentifier), std::make_shared<Token>('='), Word::create("j", Token::kIdentifier), std::make_shared<Token>('-'), std::make_shared<lexer::Number>(1), std::make_shared<Token>(';'),
        Word::create("while", Token::kWhile), std::make_shared<Token>('('), Word::create("a", Token::kIdentifier), std::make_shared<Token>('['), Word::create("j", Token::kIdentifier), std::make_shared<Token>(']'), std::make_shared<Token>('>'), Word::create("v", Token::kIdentifier), std::make_shared<Token>(')'), std::make_shared<Token>(';'),
        Word::create("if", Token::kIf), std::make_shared<Token>('('), Word::create("i", Token::kIdentifier), Word::greater_equal, Word::create("j", Token::kIdentifier), std::make_shared<Token>(')'), Word::create("break", Token::kBreak), std::make_shared<Token>(';'),
        Type::integer, Word::create("x", Token::kIdentifier), std::make_shared<Token>('='), Word::create("a", Token::kIdentifier), std::make_shared<Token>('['), Word::create("i", Token::kIdentifier), std::make_shared<Token>(']'), std::make_shared<Token>(';'),
        Word::create("a", Token::kIdentifier), std::make_shared<Token>('['), Word::create("i", Token::kIdentifier), std::make_shared<Token>(']'), std::make_shared<Token>('='), Word::create("a", Token::kIdentifier), std::make_shared<Token>('['), Word::create("j", Token::kIdentifier), std::make_shared<Token>(']'), std::make_shared<Token>(';'),
        Word::create("a", Token::kIdentifier), std::make_shared<Token>('['), Word::create("j", Token::kIdentifier), std::make_shared<Token>(']'), std::make_shared<Token>('='), Word::create("x", Token::kIdentifier), std::make_shared<Token>(';'),
        std::make_shared<Token>('}'),
        std::make_shared<Token>('}'),
      }
    },
  };

  for (auto &tc : test_cases) {
    std::stringstream source(tc.source);
    auto lex = lexer::Lexer::create(source);
    for (const auto& tk: tc.want) {
      auto tok = lex->scan();
      ASSERT_EQ(tok->to_string(), tk->to_string());
      ASSERT_EQ(tok->tag(), tk->tag());
    }
  }
}
