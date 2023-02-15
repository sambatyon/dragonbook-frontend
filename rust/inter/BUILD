load("@rules_rust//rust:defs.bzl", "rust_library", "rust_test")

rust_library(
  name = "inter",
  srcs = [
    "inter.rs",
    "expression.rs",
    "statement.rs",
  ],
  deps = [
    "//rust/lexer",
    "@crate_index//:once_cell",
  ],
  visibility = ["//rust:__subpackages__"]
)

rust_test(
  name = "inter_test",
  crate = ":inter",
  deps = [
  ],
)
