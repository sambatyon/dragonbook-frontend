cc_library(
  name = "lexer",
  srcs = [
    "lexer.cpp",
    "token.cpp",
    "type.cpp",
    "word.cpp",
  ],
  hdrs = [
    "array.hpp",
    "lexer.hpp",
    "numeric.hpp",
    "token.hpp",
    "type.hpp",
    "word.hpp",
  ],
  visibility = [
  "//cpp:__subpackages__",
  ],
)

cc_test(
  name = "lexer_tests",
  srcs = [
    "lexer_test.cpp",
  ],
  deps = [
    ":lexer",
    "@com_google_googletest//:gtest_main"
  ],
  data = [],
  size = "small",
)
