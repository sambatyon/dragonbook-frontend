load("@rules_rust//rust:defs.bzl", "rust_library", "rust_test")

rust_library(
  name = "lexer",
  srcs = [
    "lexer.rs",
    "tokens.rs",
  ],
  deps = [
    "@crate_index//:once_cell",
  ],
  visibility = ["//rust:__subpackages__"],
)

rust_test(
  name = "lexer_test",
  crate = ":lexer",
  deps = [
     "@crate_index//:stringreader",
  ],
)
