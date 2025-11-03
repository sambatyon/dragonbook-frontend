cc_library(
  name = "lexer",
  srcs = [
    "token.cc",
    "lexer.cc",
  ],
  hdrs = [
    "lexer.hh",
    "token.hh",
  ],
  visibility = [
  "//cpp:__subpackages__",
  ],
)

cc_test(
  name = "lexer_tests",
  srcs = [
    "lexer_test.cc",
  ],
  deps = [
    ":lexer",
    "@googletest//:gtest_main"
  ],
  data = [],
  size = "small",
)

