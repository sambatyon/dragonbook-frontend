load("@rules_python//python:py_binary.bzl", "py_binary")
load("@rules_python//python:py_library.bzl", "py_library")
load("//tools/python/pytest:defs.bzl", "pytest_test")

py_library(
  name = "dragon",
  srcs = [
    "dragon/__init__.py",
    "dragon/lexer/__init__.py",
    "dragon/lexer/lexer.py",
    "dragon/lexer/tokens.py",
    "dragon/ast/__init__.py",
    "dragon/ast/expression.py",
    "dragon/ast/statement.py",
    "dragon/ast/ast.py",
    "dragon/parser/__init__.py",
    "dragon/parser/parser.py",
  ],
)

pytest_test(
  name = "test_dragon",
  srcs = [
    "dragon/lexer/test_lexer.py",
    "dragon/ast/test_expression.py",
    "dragon/ast/test_statement.py",
    "dragon/parser/test_parser.py",
  ],
  deps = [
    ":dragon",
  ],
  legacy_create_init = 0,
)

py_binary(
  name = "frontend",
  deps = [":dragon"],
  srcs = ["frontend.py"],
  main = "frontend.py",
)
