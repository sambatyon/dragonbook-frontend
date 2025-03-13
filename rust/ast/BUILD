load("@rules_rust//rust:defs.bzl", "rust_library", "rust_test")

rust_library(
  name = "ast",
  srcs = [
    "ast.rs",
    "expression.rs",
    "statement.rs",
  ],
  deps = [
    "//rust/lexer",
    "@crates//:once_cell",
  ],
  visibility = ["//rust:__subpackages__"]
)

# Run this test with  --test_arg="--test-threads=1" since the label generator
# may get reset.
rust_test(
  name = "ast_test",
  crate = ":ast",
  deps = [
  ],
)
