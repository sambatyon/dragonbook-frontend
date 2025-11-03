#include <gtest/gtest.h>

#include "cpp/dragon/lexer/lexer.hh"
#include "cpp/dragon/lexer/token.hh"

#include <string>
#include <vector>

struct LexerTC {
  std::string source;
  std::vector<dragon::lexer::Token> want;
};

TEST(TestLexer, LexerTests) {
  namespace lx = dragon::lexer;
  namespace tag = dragon::lexer::tag;
  namespace op = dragon::lexer::operators;
  namespace typ = dragon::lexer::types;
  std::vector<LexerTC> test_cases{
    LexerTC{"&", std::vector<lx::Token>{lx::Token('&')}},
    LexerTC{"&&", std::vector<lx::Token>{op::kAndWord}},
    LexerTC{"|", std::vector<lx::Token>{lx::Token('|')}},
    LexerTC{"||", std::vector<lx::Token>{op::kOrWord}},
    LexerTC{"!", std::vector<lx::Token>{lx::Token('!')}},
    LexerTC{"!=", std::vector<lx::Token>{op::kNotEqual}},
    LexerTC{"<", std::vector<lx::Token>{lx::Token('<')}},
    LexerTC{"<=", std::vector<lx::Token>{op::kLessEqual}},
    LexerTC{">", std::vector<lx::Token>{lx::Token('>')}},
    LexerTC{">=", std::vector<lx::Token>{op::kGreaterEqual}},
    LexerTC{"1982", std::vector<lx::Token>{lx::Token(tag::kInteger, 1982ll)}},
    LexerTC{"1982.2981", std::vector<lx::Token>{lx::Token(tag::kReal, 1982.2981)}},
    LexerTC{"Iden7ifer23", std::vector<lx::Token>{
      lx::Token(tag::kIdentifier, "Iden7ifer23")
    }},
    LexerTC{
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
      std::vector<lx::Token>{
        lx::Token('{'),
        typ::kInt, lx::Token(tag::kIdentifier, "i"), lx::Token(';'),
        typ::kInt, lx::Token(tag::kIdentifier, "j"), lx::Token(';'),
        typ::kFloat, lx::Token(tag::kIdentifier, "v"), lx::Token(';'),
        typ::kFloat, lx::Token('['), lx::Token(tag::kInteger, 100ll), lx::Token(']'), lx::Token(tag::kIdentifier, "a"), lx::Token(';'),
        lx::Token(tag::kWhile, "while"), lx::Token('('), op::kTrueWord, lx::Token(')'), lx::Token('{'),
        lx::Token(tag::kDo, "do"), lx::Token(tag::kIdentifier, "i"), lx::Token('='), lx::Token(tag::kIdentifier, "i"), lx::Token('+'), lx::Token(tag::kInteger, 1ll), lx::Token(';'),
        lx::Token(tag::kWhile, "while"), lx::Token('('), lx::Token(tag::kIdentifier, "a"), lx::Token('['), lx::Token(tag::kIdentifier, "i"), lx::Token(']'), lx::Token('<'), lx::Token(tag::kIdentifier, "v"), lx::Token(')'), lx::Token(';'),
        lx::Token(tag::kDo, "do"), lx::Token(tag::kIdentifier, "j"), lx::Token('='), lx::Token(tag::kIdentifier, "j"), lx::Token('-'), lx::Token(tag::kInteger, 1ll), lx::Token(';'),
        lx::Token(tag::kWhile, "while"), lx::Token('('), lx::Token(tag::kIdentifier, "a"), lx::Token('['), lx::Token(tag::kIdentifier, "j"), lx::Token(']'), lx::Token('>'), lx::Token(tag::kIdentifier, "v"), lx::Token(')'), lx::Token(';'),
        lx::Token(tag::kIf, "if"), lx::Token('('), lx::Token(tag::kIdentifier, "i"), op::kGreaterEqual, lx::Token(tag::kIdentifier, "j"), lx::Token(')'), lx::Token(tag::kBreak, "break"), lx::Token(';'),
        typ::kInt, lx::Token(tag::kIdentifier, "x"), lx::Token('='), lx::Token(tag::kIdentifier, "a"), lx::Token('['), lx::Token(tag::kIdentifier, "i"), lx::Token(']'), lx::Token(';'),
        lx::Token(tag::kIdentifier, "a"), lx::Token('['), lx::Token(tag::kIdentifier, "i"), lx::Token(']'), lx::Token('='), lx::Token(tag::kIdentifier, "a"), lx::Token('['), lx::Token(tag::kIdentifier, "j"), lx::Token(']'), lx::Token(';'),
        lx::Token(tag::kIdentifier, "a"), lx::Token('['), lx::Token(tag::kIdentifier, "j"), lx::Token(']'), lx::Token('='), lx::Token(tag::kIdentifier, "x"), lx::Token(';'),
        lx::Token('}'),
        lx::Token('}'),
      },
    },
  };

  for (auto &tc : test_cases) {
    std::stringstream source(tc.source);
    lx::Lexer lex(source);
    for (const auto& tk: tc.want) {
      auto tok = lex.scan();
      ASSERT_EQ(tok.to_string(), tk.to_string());
      ASSERT_EQ(tok.tag(), tk.tag());
    }
  }
}
