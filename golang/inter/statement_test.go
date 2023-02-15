package inter

import (
	"dragonbook/lexer"
	"strings"
	"testing"
)

func intConstant(val int64) *Constant {
	cst, _ := NewIntConstant(&lexer.Integer{val})
	return cst
}

var stmtTests = []struct {
	stmt Statement
	want string
}{
	{
		&AssignStmt{
			&Identifier{lexer.NewWord(lexer.ID, "x"), lexer.IntType(), 4},
			intConstant(42),
		},
		"\tx = 42\n",
	},
	{
		&AssingArrayStmt{
			&Identifier{lexer.NewWord(lexer.ID, "x"), lexer.IntType(), 4},
			intConstant(0),
			intConstant(42),
		},
		"\tx [ 0 ] = 42\n",
	},
	{
		&StmtSeq{
			&AssignStmt{
				&Identifier{lexer.NewWord(lexer.ID, "x"), lexer.IntType(), 4},
				intConstant(42),
			},
			&AssingArrayStmt{
				&Identifier{lexer.NewWord(lexer.ID, "x"), lexer.IntType(), 4},
				intConstant(0),
				intConstant(42),
			},
		},
		"\tx = 42\nL3:\tx [ 0 ] = 42\n",
	},
	{
		&IfStmt{
			&Identifier{lexer.NewWord(lexer.ID, "b"), lexer.BoolType(), 4},
			&AssignStmt{
				&Identifier{lexer.NewWord(lexer.ID, "x"), lexer.IntType(), 4},
				intConstant(0),
			},
		},
		"\tiffalse b goto L2\nL3:\tx = 0\n",
	},
	{
		&ElseStmt{
			&Identifier{lexer.NewWord(lexer.ID, "b"), lexer.BoolType(), 4},
			&AssignStmt{
				&Identifier{lexer.NewWord(lexer.ID, "x"), lexer.IntType(), 4},
				intConstant(0),
			},
			&AssignStmt{
				&Identifier{lexer.NewWord(lexer.ID, "x"), lexer.IntType(), 4},
				intConstant(42),
			},
		},
		"\tiffalse b goto L4\nL3:\tx = 0\n\tgoto L2\nL4:\tx = 42\n",
	},
	{
		&WhileStmt{
			&Identifier{lexer.NewWord(lexer.ID, "b"), lexer.BoolType(), 4},
			&AssignStmt{
				&Identifier{lexer.NewWord(lexer.ID, "x"), lexer.IntType(), 4},
				intConstant(0),
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
				intConstant(0),
			},
			0,
		},
		"\tx = 0\nL3:\tif b goto L1\n",
	},
}

func TestStatements(t *testing.T) {
	for _, test := range stmtTests {
		ResetLabels()
		ResetTempCount()
		begin := NewLabel()
		after := NewLabel()
		var b strings.Builder
		test.stmt.Generate(&b, begin, after)
		if b.String() != test.want {
			t.Fatalf("Expected '%s' got '%s'", test.want, b.String())
		}
	}
}
