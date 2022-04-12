package lexer

import "fmt"

type Tag int

const (
	AND Tag = 256 + iota
	BASIC
	BREAK
	DO
	ELSE
	EQ
	FALSE
	GE
	ID
	IF
	INDEX
	LE
	MINUS
	NE
	INTEGER
	OR
	REAL
	TEMP
	TRUE
	WHILE
)

type Token interface {
	Tag() Tag
	String() string
}

type Tok struct {
	tag Tag
}

func (t *Tok) Tag() Tag {
	return t.tag
}

func (t *Tok) String() string {
	return string(rune(t.tag))
}

type Integer struct {
	Value int64
}

func (i *Integer) Tag() Tag {
	return INTEGER
}

func (i *Integer) String() string {
	return fmt.Sprintf("%d", i.Value)
}

type Real struct {
	Value float64
}

func (r *Real) Tag() Tag {
	return REAL
}

func (i *Real) String() string {
	return fmt.Sprintf("%f", i.Value)
}

type Word struct {
	tag    Tag
	lexeme string
}

func (w *Word) Tag() Tag {
	return w.tag
}

func (w *Word) String() string {
	return w.lexeme
}

var andWord = &Word{tag: AND, lexeme: "&&"}

func AndWord() *Word {
	return andWord
}

var orWord = &Word{tag: OR, lexeme: "||"}

func OrWord() *Word {
	return orWord
}

var eqWord = &Word{tag: EQ, lexeme: "=="}

func EqWord() *Word {
	return eqWord
}

var neWord = &Word{tag: NE, lexeme: "!="}

func NeWord() *Word {
	return neWord
}

var leWord = &Word{tag: LE, lexeme: "<="}

func LeWord() *Word {
	return leWord
}

var geWord = &Word{tag: GE, lexeme: ">="}

func GeWord() *Word {
	return geWord
}

var minusWord = &Word{tag: MINUS, lexeme: "minus"}

func MinusWord() *Word {
	return minusWord
}

var trueWord = &Word{tag: TRUE, lexeme: "true"}

func TrueWord() *Word {
	return trueWord
}

var falseWord = &Word{tag: FALSE, lexeme: "false"}

func FalseWord() *Word {
	return falseWord
}

var tempWord = &Word{tag: TEMP, lexeme: "t"}

func TempWord() *Word {
	return tempWord
}

type Type struct {
	tag    Tag
	lexeme string
	Width  int
}

func (t *Type) Tag() Tag {
	return t.tag
}

func (t *Type) String() string {
	return t.lexeme
}

func (t *Type) Numeric() bool {
	return t == IntType() || t == FloatType() || t == CharType()
}

var intType = &Type{tag: BASIC, lexeme: "int", Width: 4}

func IntType() *Type {
	return intType
}

var floatType = &Type{tag: BASIC, lexeme: "float", Width: 8}

func FloatType() *Type {
	return floatType
}

var charType = &Type{tag: BASIC, lexeme: "char", Width: 1}

func CharType() *Type {
	return charType
}

var boolType = &Type{tag: BASIC, lexeme: "bool", Width: 1}

func BoolType() *Type {
	return boolType
}
