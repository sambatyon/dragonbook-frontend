package lexer

import (
	"strings"
	"testing"
)

var lexerTests = []struct {
	source string
}{
	{"&"},
}

func TestLexer(t *testing.T) {
	for _, test := range lexerTests {
		s := strings.NewReader(test.source)
		l := NewLexer(s)
		_, err := l.Scan()
		if err != nil {
			t.Fatalf("Error scanning input '%s': %v", test.source, err)
		}
	}
}
