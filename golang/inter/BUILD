load("@rules_go//go:def.bzl", "go_library", "go_test")

go_library(
  name = "inter",
  srcs = [
    "inter.go",
    "expression.go",
    "statement.go",
  ],
  deps = [
    "//golang/lexer:lexer",
  ],
  importpath = "dragonbook/inter",
  visibility = ["//golang:__subpackages__"],
)

go_test(
  name = "inter_test",
  srcs = [
    "expression_test.go",
    "statement_test.go"
  ],
  embed = [":inter"],
)
