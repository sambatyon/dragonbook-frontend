#include <gtest/gtest.h>

#include "cpp/inter/access.hpp"
#include "cpp/inter/and.hpp"
#include "cpp/inter/arithmetic-operator.hpp"
#include "cpp/inter/expression.hpp"
#include "cpp/inter/identifier.hpp"
#include "cpp/inter/node.hpp"
#include "cpp/inter/not.hpp"
#include "cpp/inter/or.hpp"
#include "cpp/inter/relational.hpp"
#include "cpp/inter/temporary.hpp"
#include "cpp/inter/unary.hpp"

#include "cpp/lexer/token.hpp"
#include "cpp/lexer/word.hpp"
#include "cpp/lexer/type.hpp"

#include <string>
#include <sstream>
#include <vector>

using inter::Access;
using inter::And;
using inter::Arithmetic;
using inter::Expression;
using inter::Identifier;
using inter::Node;
using inter::Not;
using inter::Or;
using inter::Relational;
using inter::Temporary;
using inter::UnaryOperator;

using lexer::Word;
using lexer::Token;

using symbols::Type;

namespace {
struct TestCase {
  std::shared_ptr<Expression> expr;
  std::string str;
  std::string gen;
  std::string red;
};
}

TEST(TestExpr, InterTests) {
  std::vector<TestCase> test_cases{
    TestCase{
      Identifier::create(Word::create("example", Token::kIdentifier), Type::integer, 4),
      "example",
      "",
      "",
    },
    TestCase{
      Temporary::create(Type::integer),
      "t1",
      "",
      "",
    },
    TestCase{
      Arithmetic::create(
        Token::create('+'),
        Identifier::create(Word::create("x", Token::kIdentifier), Type::integer, 4),
        Identifier::create(Word::create("y", Token::kIdentifier), Type::integer, 4)
      ),
      "x + y",
      "",
      "\tt1 = x + y\n",
    },
    TestCase{
      UnaryOperator::create(
        Token::create('-'),
        Identifier::create(Word::create("x", Token::kIdentifier), Type::integer, 4)
      ),
      "- x",
      "",
      "\tt1 = - x\n",
    },
    TestCase{
      Access::create(
        Identifier::create(Word::create("arr", Token::kIdentifier), Type::real, 4),
        Identifier::create(Word::create("x", Token::kIdentifier), Type::integer, 4),
        Type::real
      ),
      "arr[ x ]",
      "",
      "\tt1 = arr[ x ]\n",
    },
    TestCase{
      Or::create(
        Word::or_word,
        Identifier::create(Word::create("x", Token::kIdentifier), Type::boolean, 4),
        Identifier::create(Word::create("y", Token::kIdentifier), Type::boolean, 4)
      ),
      "x || y",
      "\tif x goto L3\n\tiffalse y goto L1\nL3:\tt1 = true\n\tgoto L2\nL1:\tt1 = false\nL2:",
      "",
    },
    TestCase{
      And::create(
        Word::and_word,
        Identifier::create(Word::create("x", Token::kIdentifier), Type::boolean, 4),
        Identifier::create(Word::create("y", Token::kIdentifier), Type::boolean, 4)
      ),
      "x && y",
      "\tiffalse x goto L1\n\tiffalse y goto L1\n\tt1 = true\n\tgoto L2\nL1:\tt1 = false\nL2:",
      "",
    },
    TestCase{
      Relational::create(
        Word::equal,
        Identifier::create(Word::create("x", Token::kIdentifier), Type::boolean, 4),
        Identifier::create(Word::create("y", Token::kIdentifier), Type::boolean, 4)
      ),
      "x == y",
      "\tiffalse x == y goto L1\n\tt1 = true\n\tgoto L2\nL1:\tt1 = false\nL2:",
      "",
    },
  };

  for (const auto &tc : test_cases) {
    Node::reset_labels();
    Temporary::reset_temp_count();

    ASSERT_EQ(tc.expr->to_string(), tc.str);
    std::stringstream ss;
    tc.expr->gen(ss);
    ASSERT_EQ(ss.str(), tc.gen);
    ss.str("");
    tc.expr->reduce(ss);
    ASSERT_EQ(ss.str(), tc.red);
  }
}
