import io
import pytest
from typing import List

import dragon.lexer as lexer
from dragon.lexer import tokens


@pytest.mark.parametrize("source,want", [
    (";", [tokens.Token(ord(";"))]),
    ("&", [tokens.Token(ord("&"))]),
    ("&&", [tokens.Word.AND]),
    ("|", [tokens.Token(ord('|'))]),
    ("||", [tokens.Word.OR]),
    ("!", [tokens.Token(ord('!'))]),
    ("!=", [tokens.Word.NE]),
    ("<", [tokens.Token(ord('<'))]),
    ("<=", [tokens.Word.LE]),
    (">", [tokens.Token(ord('>'))]),
    (">=", [tokens.Word.GE]),
    ("1982", [tokens.Integer(1982)]),
    ("1981.2981", [tokens.Real(1981.2981)]),
    ("Iden7ifer23", [tokens.Word("Iden7ifer23", tokens.ID)]),
    ("""
        {
          int i; int j; float v; float[100] a;
          while (true) {
            do i = i + 1; while(a[i] < v);
            do j = j - 1; while(a[j] > v);
            if (i >= j) break;
            int x = a[i];
            a[i] = a[j];
            a[j] = x;
          }
        }
     """,
    [
        tokens.Token(ord('{')),
        tokens.Type.INT, tokens.Word("i", tokens.ID), tokens.Token(ord(';')),
        tokens.Type.INT, tokens.Word("j", tokens.ID), tokens.Token(ord(';')),
        tokens.Type.FLOAT, tokens.Word("v", tokens.ID), tokens.Token(ord(';')),
        tokens.Type.FLOAT, tokens.Token(ord('[')), tokens.Integer(100), tokens.Token(ord(']')), tokens.Word("a", tokens.ID), tokens.Token(ord(';')),
        tokens.Word("while", tokens.WHILE), tokens.Token(ord('(')), tokens.Word.TRUE, tokens.Token(ord(')')), tokens.Token(ord('{')),
        tokens.Word("do", tokens.DO), tokens.Word("i", tokens.ID), tokens.Token(ord('=')), tokens.Word("i", tokens.ID), tokens.Token(ord('+')), tokens.Integer(1), tokens.Token(ord(';')),
        tokens.Word("while", tokens.WHILE), tokens.Token(ord('(')), tokens.Word("a", tokens.ID), tokens.Token(ord('[')), tokens.Word("i", tokens.ID), tokens.Token(ord(']')), tokens.Token(ord('<')), tokens.Word("v", tokens.ID), tokens.Token(ord(')')), tokens.Token(ord(';')),
        tokens.Word("do", tokens.DO), tokens.Word("j", tokens.ID), tokens.Token(ord('=')), tokens.Word("j", tokens.ID), tokens.Token(ord('-')), tokens.Integer(1), tokens.Token(ord(';')),
        tokens.Word("while", tokens.WHILE), tokens.Token(ord('(')), tokens.Word("a", tokens.ID), tokens.Token(ord('[')), tokens.Word("j", tokens.ID), tokens.Token(ord(']')), tokens.Token(ord('>')), tokens.Word("v", tokens.ID), tokens.Token(ord(')')), tokens.Token(ord(';')),
        tokens.Word("if", tokens.IF), tokens.Token(ord('(')), tokens.Word("i", tokens.ID), tokens.Word.GE, tokens.Word("j", tokens.ID), tokens.Token(ord(')')), tokens.Word("break", tokens.BREAK), tokens.Token(ord(';')),
        tokens.Type.INT, tokens.Word("x", tokens.ID), tokens.Token(ord('=')), tokens.Word("a", tokens.ID), tokens.Token(ord('[')), tokens.Word("i", tokens.ID), tokens.Token(ord(']')), tokens.Token(ord(';')),
        tokens.Word("a", tokens.ID), tokens.Token(ord('[')), tokens.Word("i", tokens.ID), tokens.Token(ord(']')), tokens.Token(ord('=')), tokens.Word("a", tokens.ID), tokens.Token(ord('[')), tokens.Word("j", tokens.ID), tokens.Token(ord(']')), tokens.Token(ord(';')),
        tokens.Word("a", tokens.ID), tokens.Token(ord('[')), tokens.Word("j", tokens.ID), tokens.Token(ord(']')), tokens.Token(ord('=')), tokens.Word("x", tokens.ID), tokens.Token(ord(';')),
        tokens.Token(ord('}')),
        tokens.Token(ord('}'))
    ])
])
def test_lexer(source: str, want: List[tokens.Token]):
  lex = lexer.Lexer(io.StringIO(source))
  for tk in want:
    tok = lex.scan()
    assert str(tk) == str(tok)
    assert tk.tag == tok.tag
