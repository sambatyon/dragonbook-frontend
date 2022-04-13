package lexer

import (
	"strings"
	"testing"
)

var lexerTests = []struct {
	source string
	want   []Token
}{
	{"&", []Token{
		&Tok{tag: Tag('&')},
	}},
	{"&&", []Token{
		AndWord(),
	}},
	{"|", []Token{
		&Tok{tag: Tag('|')},
	}},
	{"||", []Token{
		OrWord(),
	}},
	{"!", []Token{
		&Tok{tag: Tag('!')},
	}},
	{"!=", []Token{
		NeWord(),
	}},
	{"<", []Token{
		&Tok{tag: Tag('<')},
	}},
	{"<=", []Token{
		LeWord(),
	}},
	{">", []Token{
		&Tok{tag: Tag('>')},
	}},
	{">=", []Token{
		GeWord(),
	}},
	{"1982", []Token{
		&Integer{Value: 1982},
	}},
	{"1982.2981", []Token{
		&Real{Value: 1982.2981},
	}},
	{"Iden7ifer23", []Token{
		&Word{tag: ID, lexeme: "Iden7ifer23"},
	}},
	{`{
			int i; int j; float v; float[100] a;
			while (true) {
				do i = i + 1; while(a[i] < v);
				do j = j - 1; while(a[j] > v);
				if (i >= j) break;
				x = a[i];
				a[i] = a[j];
				a[j] = x;
			}
		}`, []Token{
		&Tok{tag: Tag('{')},
		IntType(), &Word{tag: ID, lexeme: "i"}, &Tok{tag: Tag(';')},
		IntType(), &Word{tag: ID, lexeme: "j"}, &Tok{tag: Tag(';')},
		FloatType(), &Word{tag: ID, lexeme: "v"}, &Tok{tag: Tag(';')},
		FloatType(), &Tok{tag: Tag('[')}, &Integer{Value: 100}, &Tok{tag: Tag(']')}, &Word{tag: ID, lexeme: "a"}, &Tok{tag: Tag(';')},
		&Word{tag: WHILE, lexeme: "while"}, &Tok{tag: Tag('(')}, TrueWord(), &Tok{tag: Tag(')')}, &Tok{tag: Tag('{')},
		&Word{tag: DO, lexeme: "do"},
		&Word{tag: ID, lexeme: "i"}, &Tok{tag: Tag('=')}, &Word{tag: ID, lexeme: "i"}, &Tok{tag: Tag('+')}, &Integer{Value: 1}, &Tok{tag: Tag(';')},
		&Word{tag: WHILE, lexeme: "while"}, &Tok{tag: Tag('(')},
		&Word{tag: ID, lexeme: "a"}, &Tok{tag: Tag('[')}, &Word{tag: ID, lexeme: "i"}, &Tok{tag: Tag(']')}, &Tok{tag: Tag('<')}, &Word{tag: ID, lexeme: "v"},
		&Tok{tag: Tag(')')}, &Tok{tag: Tag(';')},
		&Word{tag: DO, lexeme: "do"},
		&Word{tag: ID, lexeme: "j"}, &Tok{tag: Tag('=')}, &Word{tag: ID, lexeme: "j"}, &Tok{tag: Tag('-')}, &Integer{Value: 1}, &Tok{tag: Tag(';')},
		&Word{tag: WHILE, lexeme: "while"}, &Tok{tag: Tag('(')},
		&Word{tag: ID, lexeme: "a"}, &Tok{tag: Tag('[')}, &Word{tag: ID, lexeme: "j"}, &Tok{tag: Tag(']')}, &Tok{tag: Tag('>')}, &Word{tag: ID, lexeme: "v"},
		&Tok{tag: Tag(')')}, &Tok{tag: Tag(';')},
		&Word{tag: IF, lexeme: "if"}, &Tok{tag: Tag('(')}, &Word{tag: ID, lexeme: "i"}, GeWord(), &Word{tag: ID, lexeme: "j"}, &Tok{tag: Tag(')')},
		&Word{tag: BREAK, lexeme: "break"}, &Tok{tag: Tag(';')},
		&Word{tag: ID, lexeme: "x"}, &Tok{tag: Tag('=')}, &Word{tag: ID, lexeme: "a"}, &Tok{tag: Tag('[')},
		&Word{tag: ID, lexeme: "i"},
		&Tok{tag: Tag(']')}, &Tok{tag: Tag(';')},
		&Word{tag: ID, lexeme: "a"}, &Tok{tag: Tag('[')}, &Word{tag: ID, lexeme: "i"}, &Tok{tag: Tag(']')}, &Tok{tag: Tag('=')},
		&Word{tag: ID, lexeme: "a"}, &Tok{tag: Tag('[')}, &Word{tag: ID, lexeme: "j"}, &Tok{tag: Tag(']')}, &Tok{tag: Tag(';')},
		&Word{tag: ID, lexeme: "a"}, &Tok{tag: Tag('[')}, &Word{tag: ID, lexeme: "j"}, &Tok{tag: Tag(']')}, &Tok{tag: Tag('=')},
		&Word{tag: ID, lexeme: "x"}, &Tok{tag: Tag(';')},
		&Tok{tag: Tag('}')},
		&Tok{tag: Tag('}')},
	}},
}

func TestLexer(t *testing.T) {
	for _, test := range lexerTests {
		s := strings.NewReader(test.source)
		l := NewLexer(s)
		for _, tk := range test.want {
			tok, err := l.Scan()
			if err != nil {
				t.Fatalf("Error scanning input '%s': %v", test.source, err)
			}
			if tok.String() != tk.String() || tok.Tag() != tk.Tag() {
				t.Fatalf("Expected token {tag: %d str: %s} does not match {tag: %d str: %s}",
					tk.Tag(), tk.String(), tok.Tag(), tok.String())
			}
		}
	}
}
