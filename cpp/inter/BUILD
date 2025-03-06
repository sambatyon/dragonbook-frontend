cc_library(
  name = "inter",
  srcs = [
    "and.cpp",
    "constant.cpp",
    "expression.cpp",
    "logical.cpp",
    "node.cpp",
    "or.cpp",
    "relational.cpp",
    "statement-sequence.cpp",
    "statement.cpp",
    "temporary.cpp",
  ],
  hdrs = [
    "access.hpp",
    "and.hpp",
    "arithmetic-operator.hpp",
    "break.hpp",
    "constant.hpp",
    "do.hpp",
    "else.hpp",
    "expression.hpp",
    "identifier.hpp",
    "if.hpp",
    "logical.hpp",
    "node.hpp",
    "not.hpp",
    "operator.hpp",
    "or.hpp",
    "relational.hpp",
    "set-elem.hpp",
    "set.hpp",
    "statement-sequence.hpp",
    "statement.hpp",
    "temporary.hpp",
    "unary.hpp",
    "while.hpp",
  ],
  deps = [
    "//cpp/lexer:lexer",
  ],
  visibility = [
  "//cpp:__subpackages__",
  ],
)

cc_test(
  name = "inter_tests",
  srcs = [
    "expr_tests.cpp",
    "stmt_tests.cpp",
  ],
  deps = [
    ":inter",
    "@googletest//:gtest_main"
  ],
  data = [],
  size = "small",
)
