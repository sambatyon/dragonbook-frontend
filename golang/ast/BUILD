load("@rules_go//go:def.bzl", "go_library", "go_test")

go_library(
  name = "ast",
  srcs = [
    "ast.go",
    "expression.go",
    "statement.go",
  ],
  deps = [
    "//golang/lexer:lexer",
  ],
  importpath = "dragonbook/ast",
  visibility = ["//golang:__subpackages__"],
)

go_test(
  name = "ast_test",
  srcs = [
    "expression_test.go",
    "statement_test.go"
  ],
  embed = [":ast"],
)
