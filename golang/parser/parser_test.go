package parser

import (
	"dragonbook/ast"
	"dragonbook/lexer"
	"strings"
	"testing"
)

var parserTests = []struct {
	source string
	want   string
}{
	{
		`{}`,
		`L1:L2:`,
	},
	{
		`{int i;}`,
		`L1:L2:`,
	},
	{
		`{int i;float f;bool[100] b;}`,
		`L1:L2:`,
	},
	{
		`{int i; i = 10;}`,
		`L1:	i = 10
L2:`,
	},
	{
		`{int i; i = i + 10;}`,
		`L1:	i = i + 10
L2:`,
	},
	{
		`{int i;int[20] arr; i = 10; arr[i] = 10;}`,
		`L1:	i = 10
L3:	t1 = i * 4
	arr [ t1 ] = 10
L2:`,
	},
	{
		`{int i; int j; bool a; i = i + 10; j = 11; a = i == j;}`,
		`L1:	i = i + 10
L3:	j = 11
L4:	iffalse i == j goto L5
	t1 = true
	goto L6
L5:	t1 = false
L6:	a = t1
L2:`,
	},
	{
		`{int i; int j; j = 12; while (i > j) i = i + 1;}`,
		`L1:	j = 12
L3:	iffalse i > j goto L2
L4:	i = i + 1
	goto L3
L2:`,
	},
	{
		`{int i; int j; j = 12; do i = i + 1; while (i > j);}`,
		`L1:	j = 12
L3:	i = i + 1
L4:	if i > j goto L3
L2:`,
	},
	{
		`{int i; int j; while (true) i = i + 1;}`,
		`L1:L3:	i = i + 1
	goto L1
L2:`,
	},
	{
		`{while (true) {break;} }`,
		`L1:L3:	goto L2
	goto L1
L2:`,
	},
	{
		`{int i; int j; i = 10; j = 1; while (j < i) { i = i + 1; break;} }`,
		`L1:	i = 10
L3:	j = 1
L4:	iffalse j < i goto L2
L5:	i = i + 1
L6:	goto L2
	goto L4
L2:`,
	},
	{
		`{
			int i; int j; float v; float x; float[100] a;
			while (true) {
				do i = i + 1; while (a[i] < v);
				do j = j - 1; while (a[j] > v);
				if (i >= j) break;
				x = a[i];
				a[i] = a[j];
				a[j] = x;
			}
		}`,
		`L1:L3:	i = i + 1
L5:	t1 = i * 8
	t2 = a [ t1 ]
	if t2 < v goto L3
L4:	j = j - 1
L7:	t3 = j * 8
	t4 = a [ t3 ]
	if t4 > v goto L4
L6:	iffalse i >= j goto L8
L9:	goto L2
L8:	t5 = i * 8
	x = a [ t5 ]
L10:	t6 = i * 8
	t7 = j * 8
	t8 = a [ t7 ]
	a [ t6 ] = t8
L11:	t9 = j * 8
	a [ t9 ] = x
	goto L1
L2:`,
	},
}

func TestParser(t *testing.T) {
	for _, test := range parserTests {
		ast.ResetLabels()
		ast.ResetTempCount()
		rd := strings.NewReader(test.source)
		lex := lexer.NewLexer(rd)
		p, err := NewParser(lex)
		if err != nil {
			t.Fatalf("Could not create parser: %s", err)
		}
		var b strings.Builder
		if err := p.Program(&b); err != nil {
			t.Fatalf("Error parsing program: %s", err)
		}
		if b.String() != test.want {
			t.Fatalf("Expected:\n%s\n\nbut got:\n%s", test.want, b.String())
		}
	}
}
