package inter

import (
	"dragonbook/lexer"
	"strings"
	"testing"
)

var stmtTests = []struct {
	stmt Statement
	want string
}{
	{
		&AssignStmt{
			&Identifier{lexer.NewWord(lexer.ID, "x"), lexer.IntType(), 4},
			NewIntConstant(42),
		},
		"\tx = 42\n",
	},
	{
		&AssingArrayStmt{
			&Identifier{lexer.NewWord(lexer.ID, "x"), lexer.IntType(), 4},
			NewIntConstant(0),
			NewIntConstant(42),
		},
		"\tx [ 0 ] = 42\n",
	},
	{
		&StmtSeq{
			&AssignStmt{
				&Identifier{lexer.NewWord(lexer.ID, "x"), lexer.IntType(), 4},
				NewIntConstant(42),
			},
			&AssingArrayStmt{
				&Identifier{lexer.NewWord(lexer.ID, "x"), lexer.IntType(), 4},
				NewIntConstant(0),
				NewIntConstant(42),
			},
		},
		"\tx = 42\nL3:\tx [ 0 ] = 42\n",
	},
	{
		&IfStmt{
			&Identifier{lexer.NewWord(lexer.ID, "b"), lexer.BoolType(), 4},
			&AssignStmt{
				&Identifier{lexer.NewWord(lexer.ID, "x"), lexer.IntType(), 4},
				NewIntConstant(0),
			},
		},
		"\tiffalse b goto L2\nL3:\tx = 0\n",
	},
	{
		&ElseStmt{
			&Identifier{lexer.NewWord(lexer.ID, "b"), lexer.BoolType(), 4},
			&AssignStmt{
				&Identifier{lexer.NewWord(lexer.ID, "x"), lexer.IntType(), 4},
				NewIntConstant(0),
			},
			&AssignStmt{
				&Identifier{lexer.NewWord(lexer.ID, "x"), lexer.IntType(), 4},
				NewIntConstant(42),
			},
		},
		"\tiffalse b goto L4\nL3:\tx = 0\nL4:\tx = 42\n",
	},
	{
		&WhileStmt{
			&Identifier{lexer.NewWord(lexer.ID, "b"), lexer.BoolType(), 4},
			&AssignStmt{
				&Identifier{lexer.NewWord(lexer.ID, "x"), lexer.IntType(), 4},
				NewIntConstant(0),
			},
			0,
		},
		"\tiffalse b goto L2\nL3:\tx = 0\n\tgoto L1\n",
	},
	{
		&DoStmt{
			&Identifier{lexer.NewWord(lexer.ID, "b"), lexer.BoolType(), 4},
			&AssignStmt{
				&Identifier{lexer.NewWord(lexer.ID, "x"), lexer.IntType(), 4},
				NewIntConstant(0),
			},
			0,
		},
		"\tx = 0\nL3:\tif b goto L1\n",
	},
}

func TestStatements(t *testing.T) {
	for _, test := range stmtTests {
		resetLabels()
		begin := NewLabel()
		after := NewLabel()
		var b strings.Builder
		test.stmt.Generate(&b, begin, after)
		if b.String() != test.want {
			t.Fatalf("Expected '%s' got '%s'", test.want, b.String())
		}
	}
}
