load("@rules_rust//rust:defs.bzl", "rust_library", "rust_test")

rust_library(
  name = "parser",
  srcs = [
    "parser.rs",
  ],
  deps = [
    "//rust/lexer",
    "//rust/ast",
  ],
  visibility = ["//rust:__subpackages__"],
)

rust_test(
  name = "parser_test",
  crate = ":parser",
  deps = [
    "@crates//:once_cell",
    "@crates//:stringreader",
  ],
)
