package inter

import (
	"dragonbook/lexer"
	"strings"
	"testing"
)

var exprTests = []struct {
	expr Expression
	str  string
	gen  string
	red  string
}{
	{
		&Identifier{
			id:     lexer.NewWord(lexer.ID, "example"),
			typ:    lexer.IntType(),
			Offset: 4,
		},
		"example",
		"",
		"",
	},
	{
		&Temp{
			op:  lexer.TempWord(),
			typ: lexer.IntType(),
			num: 1,
		},
		"t1",
		"",
		"",
	},
	{
		&ArithmeticOp{
			lexer.NewToken(lexer.Tag('+')),
			lexer.IntType(),
			&Identifier{lexer.NewWord(lexer.ID, "x"), lexer.IntType(), 4},
			&Identifier{lexer.NewWord(lexer.ID, "y"), lexer.IntType(), 4},
		},
		"x + y",
		"",
		"\tt1 = x + y\n",
	},
	{
		&UnaryOp{
			lexer.NewToken(lexer.Tag('-')),
			lexer.FloatType(),
			&Identifier{lexer.NewWord(lexer.ID, "x"), lexer.IntType(), 4},
		},
		"- x",
		"",
		"\tt2 = - x\n",
	},
	{
		&AccessOp{
			&Identifier{lexer.NewWord(lexer.ID, "arr"), lexer.FloatType(), 4},
			&Identifier{lexer.NewWord(lexer.ID, "x"), lexer.IntType(), 4},
			lexer.FloatType(),
		},
		"arr [x]",
		"",
		"\tt3 = arr [x]\n",
	},
	{
		&NotLogicOp{
			lexer.NewToken(lexer.Tag('!')),
			&Identifier{lexer.NewWord(lexer.ID, "x"), lexer.BoolType(), 4},
		},
		"! x",
		"\tif x goto L1\n\tt4 = true\n\tgoto L2\nL1:\tt4 = false\nL2:",
		"",
	},
	{
		&OrLogicOp{
			&Identifier{lexer.NewWord(lexer.ID, "x"), lexer.BoolType(), 4},
			&Identifier{lexer.NewWord(lexer.ID, "y"), lexer.BoolType(), 4},
		},
		"x || y",
		"\tif x goto L5\n\tiffalse y goto L3\nL5:\tt5 = true\n\tgoto L4\nL3:\tt5 = false\nL4:",
		"",
	},
	{
		&AndLogicOp{
			&Identifier{lexer.NewWord(lexer.ID, "x"), lexer.BoolType(), 4},
			&Identifier{lexer.NewWord(lexer.ID, "y"), lexer.BoolType(), 4},
		},
		"x && y",
		"\tiffalse x goto L6\n\tiffalse y goto L6\n\tt6 = true\n\tgoto L7\nL6:\tt6 = false\nL7:",
		"",
	},
}

func TestExpressions(t *testing.T) {
	for _, test := range exprTests {
		if test.expr.String() != test.str {
			t.Fatalf("Expected str: %s got: %s", test.str, test.expr.String())
		}
		var b strings.Builder
		if _, err := test.expr.Generate(&b); err != nil {
			t.Fatalf("Got error generating %s: '%v'", test.expr.String(), err)
		}
		if b.String() != test.gen {
			t.Fatalf("Expected gen: %s got: '%s'", test.gen, b.String())
		}
		b.Reset()
		if _, err := test.expr.Reduce(&b); err != nil {
			t.Fatalf("Got error reducing %s: '%v'", test.expr.String(), err)
		}
		if b.String() != test.red {
			t.Fatalf("Expected red: '%s' got: '%s'", test.red, b.String())
		}
	}
}
