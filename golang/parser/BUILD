load("@io_bazel_rules_go//go:def.bzl", "go_library", "go_test")

go_library(
  name = "parser",
  srcs = [
    "parser.go",
  ],
  deps = [
    "//golang/lexer:lexer",
    "//golang/inter:inter",
  ],
  importpath = "dragonbook/parser",
  visibility = ["//golang:__subpackages__"],
)

go_test(
  name = "parser_test",
  srcs = ["parser_test.go"],
  embed = [":parser"],
  deps = [
    "//golang/lexer:lexer",
    "//golang/inter:inter",
  ],
)
