cc_library(
  name = "parser",
  srcs = [
    "parser.cpp",
  ],
  hdrs = [
    "environment.hpp",
    "parser.hpp",
  ],
  deps = [
    "//cpp/lexer:lexer",
    "//cpp/inter:inter",
  ],
  visibility = [
    "//cpp:__subpackages__",
  ],
)

cc_test(
  name = "parser_tests",
  srcs = [
    "parser_test.cpp",
  ],
  deps = [
    ":parser",
    "@com_google_googletest//:gtest_main"
  ],
  data = [],
  size = "small",
)
