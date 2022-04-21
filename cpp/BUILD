
# TODO(arojas): Separate this in libraries and main.
cc_binary(
  name = "frontend",
  srcs = ["main.cpp"] + glob([
    "inter/*.cpp",
    "lexer/*.cpp",
    "parser/*.cpp",
    "symbols/*.cpp",
  ]) + glob([
    "inter/*.hpp",
    "lexer/*.hpp",
    "parser/*.hpp",
    "symbols/*.hpp",
  ]),
)

# TODO(arojas): Add unit tests.
