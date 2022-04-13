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
