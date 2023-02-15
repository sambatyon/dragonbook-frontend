#include <gtest/gtest.h>

#include "cpp/inter/access.hpp"
#include "cpp/inter/constant.hpp"
#include "cpp/inter/else.hpp"
#include "cpp/inter/identifier.hpp"
#include "cpp/inter/if.hpp"
#include "cpp/inter/node.hpp"
#include "cpp/inter/set.hpp"
#include "cpp/inter/set-elem.hpp"
#include "cpp/inter/statement.hpp"
#include "cpp/inter/statement-sequence.hpp"
#include "cpp/inter/temporary.hpp"

#include "cpp/lexer/token.hpp"
#include "cpp/lexer/word.hpp"
#include "cpp/lexer/type.hpp"

#include <string>
#include <sstream>
#include <vector>

using inter::Access;
using inter::Constant;
using inter::Else;
using inter::Identifier;
using inter::If;
using inter::Node;
using inter::Set;
using inter::SetElem;
using inter::Statement;
using inter::StatementSequence;
using inter::Temporary;

using lexer::Word;
using lexer::Token;

using symbols::Type;

namespace {
struct TestCase {
  std::shared_ptr<Statement> stmt;
  std::string gen;
};
}

TEST(TestStmt, InterTests) {
   std::vector<TestCase> test_cases{
    TestCase{
      Set::create(
        Identifier::create(Word::create("x", Token::kIdentifier), Type::integer, 4),
        Constant::create(42)
      ),
      "\tx = 42\n",
    },
    TestCase{
      SetElem::create(
        Access::create(
          Identifier::create(Word::create("arr", Token::kIdentifier), Type::real, 4),
          Identifier::create(Word::create("x", Token::kIdentifier), Type::integer, 4),
          Type::real
        ),
        Constant::create(42.0)
      ),
      "\tarr[ x ] = 42\n",
    },
    TestCase{
      StatementSequence::create(
        Set::create(
          Identifier::create(Word::create("x", Token::kIdentifier), Type::integer, 4),
          Constant::create(42)
        ),
        SetElem::create(
          Access::create(
            Identifier::create(Word::create("arr", Token::kIdentifier), Type::real, 4),
            Identifier::create(Word::create("x", Token::kIdentifier), Type::integer, 4),
            Type::real
          ),
          Constant::create(42.0)
        )
      ),
      "\tx = 42\nL3:\tarr[ x ] = 42\n",
    },
    TestCase{
      If::create(
        Identifier::create(Word::create("b", Token::kIdentifier), Type::boolean, 4),
        Set::create(
          Identifier::create(Word::create("x", Token::kIdentifier), Type::integer, 4),
          Constant::create(0)
        )
      ),
      "\tiffalse b goto L2\nL3:\tx = 0\n",
    },
    TestCase{
      Else::create(
        Identifier::create(Word::create("b", Token::kIdentifier), Type::boolean, 4),
        Set::create(
          Identifier::create(Word::create("x", Token::kIdentifier), Type::integer, 4),
          Constant::create(0)
        ),
        Set::create(
          Identifier::create(Word::create("x", Token::kIdentifier), Type::integer, 4),
          Constant::create(42)
        )
      ),
      "\tiffalse b goto L4\nL3:\tx = 0\n\tgoto L2\nL4:\tx = 42\n",
    },
   };

   for (auto &tc : test_cases) {
    Node::reset_labels();
    Temporary::reset_temp_count();
    auto begin = Node::new_label();
    auto after = Node::new_label();
    std::stringstream ss;
    tc.stmt->gen(ss, begin, after);
    ASSERT_EQ(ss.str(), tc.gen);
   }
}
