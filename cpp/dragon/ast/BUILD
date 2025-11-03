cc_library(
  name = "ast",
  srcs = [
    "expression.cc",
    "node.cc",
  ],
  hdrs = [
    "node.hh",
    "expression.hh",
  ],
  deps = [
    "//cpp/dragon/lexer",
    "//cpp/dragon/symbols",
  ],
  visibility = [
  "//cpp:__subpackages__",
  ],
)

cc_test(
  name = "ast_tests",
  srcs = [
    "expr_test.cc",
  ],
  deps = [
    ":ast",
    "@googletest//:gtest_main"
  ],
  data = [],
  size = "small",
)
