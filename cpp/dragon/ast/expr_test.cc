#include <gtest/gtest.h>

#include "cpp/dragon/ast/expression.hh"
#include "cpp/dragon/lexer/token.hh"

#include <memory>

namespace ast = dragon::ast;
namespace lxr = dragon::lexer;
namespace tag = dragon::lexer::tag;
namespace sy = dragon::symbols;

struct TestCase {
  std::unique_ptr<ast::Expr> expr;
  std::string str;
  std::string gen;
  std::string red;
};

TEST(TestExpr, AstTests) {
  std::vector<TestCase> tcs;
  tcs.emplace_back(
    TestCase{
      std::make_unique<ast::ID>(lxr::Token(tag::kIdentifier, "example"), sy::Type::kNumber.clone(), 4),
      "example",
      "",
      "",
    }
  );
  tcs.emplace_back(
    TestCase{
      ast::Const::num_const(314),
      "314",
      "",
      "",
    }
  );
  tcs.emplace_back(
    TestCase{
      std::make_unique<ast::Temp>(sy::Type::kNumber.clone()),
      "t1",
      "",
      "",
    }
  );
  tcs.emplace_back(
    TestCase{
      std::make_unique<ast::Arithmetic>(
        lxr::Token('+'),
        std::make_unique<ast::ID>(lxr::Token(tag::kIdentifier, "x"), sy::Type::kNumber.clone(), 4),
        std::make_unique<ast::ID>(lxr::Token(tag::kIdentifier, "y"), sy::Type::kNumber.clone(), 4)
      ),
      "x + y",
      "",
      "\tt1 = x + y\n",
    }
  );
  tcs.emplace_back(
    TestCase{
      std::make_unique<ast::Unary>(
        lxr::Token('-'),
        std::make_unique<ast::ID>(lxr::Token(tag::kIdentifier, "x"), sy::Type::kNumber.clone(), 4)
      ),
      "- x",
      "",
      "\tt1 = - x\n",
    }
  );
  tcs.emplace_back(
    TestCase{
      std::make_unique<ast::Access>(
        std::make_unique<ast::ID>(lxr::Token(tag::kIdentifier, "arr"), sy::Type::kReal.clone(), 4),
        std::make_unique<ast::ID>(lxr::Token(tag::kIdentifier, "x"), sy::Type::kNumber.clone(), 4),
        sy::Type::kReal.clone()
      ),
      "arr[ x ]",
      "",
      "\tt1 = arr[ x ]\n",
    }
  );
  tcs.emplace_back(
    TestCase{
      std::make_unique<ast::Not>(
        lxr::Token('!'),
        std::make_unique<ast::ID>(lxr::Token(tag::kIdentifier, "x"), sy::Type::kBoolean.clone(), 4)
      ),
      "! x",
      "\tif x goto L1\n\tt1 = true\n\tgoto L2\nL1:\tt1 = false\nL2:",
      "",
    }
  );
  tcs.emplace_back(
    TestCase{
      std::make_unique<ast::Or>(
        std::make_unique<ast::ID>(lxr::Token(tag::kIdentifier, "x"), sy::Type::kBoolean.clone(), 4),
        std::make_unique<ast::ID>(lxr::Token(tag::kIdentifier, "y"), sy::Type::kBoolean.clone(), 4)
      ),
      "x || y",
      "\tif x goto L3\n\tiffalse y goto L1\nL3:\tt1 = true\n\tgoto L2\nL1:\tt1 = false\nL2:",
      "",
    }
  );
  tcs.emplace_back(
    TestCase{
      std::make_unique<ast::And>(
        std::make_unique<ast::ID>(lxr::Token(tag::kIdentifier, "x"), sy::Type::kBoolean.clone(), 4),
        std::make_unique<ast::ID>(lxr::Token(tag::kIdentifier, "y"), sy::Type::kBoolean.clone(), 4)
      ),
      "x && y",
      "\tiffalse x goto L1\n\tiffalse y goto L1\n\tt1 = true\n\tgoto L2\nL1:\tt1 = false\nL2:",
      "",
    }
  );

  for (const auto &tc : tcs) {
    ast::Temp::reset_temp_count();
    ast::reset_label();
    ASSERT_EQ(tc.expr->to_string(), tc.str);
    std::stringstream ss;
    tc.expr->gen(ss);
    ASSERT_EQ(ss.str(), tc.gen);
    ss.str("");
    tc.expr->reduce(ss);
    ASSERT_EQ(ss.str(), tc.red);
  }
}
