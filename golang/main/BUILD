load("@rules_go//go:def.bzl", "go_binary")

go_binary(
  name = "frontend",
  srcs = [
    "main.go",
  ],
  deps = [
    "//golang/lexer",
    "//golang/parser",
  ]
)
