package lexer

import (
	"fmt"
	"math"
)

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
	EOF = math.MaxInt
)

type Token interface {
	Tag() Tag
	String() string
}

type Tok struct {
	tag Tag
}

var _ Token = (*Tok)(nil)

var eof Token = &Tok{EOF}

func Eof() Token {
	return eof
}

func NewToken(tag Tag) *Tok {
	return &Tok{tag: tag}
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

var _ Token = (*Integer)(nil)

func (i *Integer) Tag() Tag {
	return INTEGER
}

func (i *Integer) String() string {
	return fmt.Sprintf("%d", i.Value)
}

type Real struct {
	Value float64
}

var _ Token = (*Real)(nil)

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

var _ Token = (*Word)(nil)

func NewWord(tag Tag, lexeme string) *Word {
	return &Word{tag: tag, lexeme: lexeme}
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

var accessWord = &Word{tag: INDEX, lexeme: "[]"}

func AccessWord() *Word {
	return accessWord
}

type Type interface {
	Token
	Width() int
	Numeric() bool
}

type SimpleType struct {
	tag    Tag
	lexeme string
	width  int
}

var (
	_ Token = (*SimpleType)(nil)
	_ Type  = (*SimpleType)(nil)
)

func (t *SimpleType) Tag() Tag {
	return t.tag
}

func (t *SimpleType) String() string {
	return t.lexeme
}

func (t *SimpleType) Width() int {
	return t.width
}

func (t *SimpleType) Numeric() bool {
	return t == IntType() || t == FloatType() || t == CharType()
}

func MaxType(left Type, right Type) Type {
	if !left.Numeric() || !right.Numeric() {
		return nil
	}
	if left == FloatType() || right == FloatType() {
		return FloatType()
	}
	if left == IntType() || right == IntType() {
		return IntType()
	}

	return CharType()
}

var intType = &SimpleType{tag: BASIC, lexeme: "int", width: 4}

func IntType() *SimpleType {
	return intType
}

var floatType = &SimpleType{tag: BASIC, lexeme: "float", width: 8}

func FloatType() *SimpleType {
	return floatType
}

var charType = &SimpleType{tag: BASIC, lexeme: "char", width: 1}

func CharType() *SimpleType {
	return charType
}

var boolType = &SimpleType{tag: BASIC, lexeme: "bool", width: 1}

func BoolType() *SimpleType {
	return boolType
}

type Array struct {
	Of     Type
	Length int
}

var (
	_ Token = (*Array)(nil)
	_ Type  = (*Array)(nil)
)

func (a *Array) Tag() Tag {
	return INDEX
}

func (a *Array) String() string {
	return fmt.Sprintf("[%d]%s", a.Length, a.Of.String())
}

func (a *Array) Width() int {
	return a.Of.Width() * a.Length
}

func (a *Array) Numeric() bool {
	return false
}
