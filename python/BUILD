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
  ],
)

pytest_test(
  name = "test_lexer",
  srcs = ["dragon/lexer/test_lexer.py"],
  legacy_create_init = 0,
  deps = [
    ":dragon",
  ],
)

py_binary(
  name = "frontend",
  srcs = ["frontend.py"],
  main = "frontend.py",
  deps = [":dragon"],
)
