load("@io_bazel_rules_go//go:def.bzl", "go_library", "go_test")

go_library(
  name = "lexer",
  srcs = [
    "lexer.go",
    "tokens.go",
  ],
  importpath = "dragonbook/lexer",
  visibility = ["//golang:__subpackages__"]
)

go_test(
  name = "lexer_test",
  srcs = ["lexer_test.go"],
  embed = [":lexer"],
)
