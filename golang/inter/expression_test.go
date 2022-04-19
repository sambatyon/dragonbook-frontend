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
		`	t1 = x + y
`,
	},
	{
		&UnaryOp{
			lexer.NewToken(lexer.Tag('-')),
			lexer.FloatType(),
			&Identifier{lexer.NewWord(lexer.ID, "x"), lexer.IntType(), 4},
		},
		"- x",
		"",
		`	t1 = - x
`,
	},
	{
		&AccessOp{
			&Identifier{lexer.NewWord(lexer.ID, "arr"), lexer.FloatType(), 4},
			&Identifier{lexer.NewWord(lexer.ID, "x"), lexer.IntType(), 4},
			lexer.FloatType(),
		},
		"arr [x]",
		"",
		`	t1 = arr [x]
`,
	},
	{
		&NotLogicOp{
			lexer.NewToken(lexer.Tag('!')),
			&Identifier{lexer.NewWord(lexer.ID, "x"), lexer.BoolType(), 4},
		},
		"! x",
		`	if x goto L1
	t1 = true
	goto L2
L1:	t1 = false
L2:`,
		"",
	},
	{
		&OrLogicOp{
			&Identifier{lexer.NewWord(lexer.ID, "x"), lexer.BoolType(), 4},
			&Identifier{lexer.NewWord(lexer.ID, "y"), lexer.BoolType(), 4},
		},
		"x || y",
		`	if x goto L3
	iffalse y goto L1
L3:	t1 = true
	goto L2
L1:	t1 = false
L2:`,
		"",
	},
	{
		&AndLogicOp{
			&Identifier{lexer.NewWord(lexer.ID, "x"), lexer.BoolType(), 4},
			&Identifier{lexer.NewWord(lexer.ID, "y"), lexer.BoolType(), 4},
		},
		"x && y",
		`	iffalse x goto L1
	iffalse y goto L1
	t1 = true
	goto L2
L1:	t1 = false
L2:`,
		"",
	},
	{
		&RelationOp{
			lexer.EqWord(),
			&Identifier{lexer.NewWord(lexer.ID, "x"), lexer.BoolType(), 4},
			&Identifier{lexer.NewWord(lexer.ID, "y"), lexer.BoolType(), 4},
		},
		"x == y",
		`	iffalse x == y goto L1
	t1 = true
	goto L2
L1:	t1 = false
L2:`,
		"",
	},
}

func TestExpressions(t *testing.T) {
	for _, test := range exprTests {
		ResetLabels()
		ResetTempCount()

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
