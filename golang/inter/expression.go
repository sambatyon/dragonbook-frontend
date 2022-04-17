package inter

import (
	"dragonbook/lexer"
	"errors"
	"fmt"
	"strings"
)

type Expression interface {
	Op() lexer.Token
	Type() lexer.Type
	Generate(*strings.Builder) (Expression, error)
	Reduce(*strings.Builder) (Expression, error)
	String() string
	Jumps(b *strings.Builder, to int, from int) error
}

func EmitJumps(bd *strings.Builder, test string, to int, from int) {
	if to != 0 && from != 0 {
		Emit(bd, fmt.Sprintf("if %s goto L%d", test, to))
		Emit(bd, fmt.Sprintf("goto L%d", from))
	} else if to != 0 {
		Emit(bd, fmt.Sprintf("if %s goto L%d", test, to))
	} else if from != 0 {
		Emit(bd, fmt.Sprintf("iffalse %s goto L%d", test, from))
	}
}

type expr struct {
	op  lexer.Token
	typ lexer.Type
}

func (e *expr) Op() lexer.Token {
	return e.op
}

func (e *expr) Type() lexer.Type {
	return e.typ
}

func (e *expr) Generate(*strings.Builder) (Expression, error) {
	return e, nil
}

func (e *expr) Reduce(*strings.Builder) (Expression, error) {
	return e, nil
}

func (e *expr) String() string {
	return e.op.String()
}

func (e *expr) Jumps(b *strings.Builder, to int, from int) error {
	EmitJumps(b, e.String(), to, from)
	return nil
}

type Identifier struct {
	id     lexer.Token
	typ    lexer.Type
	Offset int
}

func (id *Identifier) Op() lexer.Token {
	return id.id
}

func (id *Identifier) Type() lexer.Type {
	return id.typ
}

func (id *Identifier) Generate(*strings.Builder) (Expression, error) {
	return id, nil
}

func (id *Identifier) Reduce(*strings.Builder) (Expression, error) {
	return id, nil
}

func (id *Identifier) String() string {
	return id.id.String()
}

func (id *Identifier) Jumps(b *strings.Builder, to int, from int) error {
	EmitJumps(b, id.String(), to, from)
	return nil
}

type Temp struct {
	op  lexer.Token
	typ lexer.Type
	num int
}

func (t *Temp) Op() lexer.Token {
	return t.op
}

func (t *Temp) Type() lexer.Type {
	return t.typ
}

func (t *Temp) Generate(*strings.Builder) (Expression, error) {
	return t, nil
}

func (t *Temp) Reduce(*strings.Builder) (Expression, error) {
	return t, nil
}

func (t *Temp) String() string {
	return fmt.Sprintf("t%d", t.num)
}

func (t *Temp) Jumps(b *strings.Builder, to int, from int) error {
	EmitJumps(b, t.String(), to, from)
	return nil
}

var tempNumber int = 0

func NewTemp(typ lexer.Type) *Temp {
	tempNumber++
	return &Temp{op: lexer.TempWord(), typ: typ, num: tempNumber}
}

type ArithmeticOp struct {
	op    lexer.Token
	typ   lexer.Type
	left  Expression
	right Expression
}

func NewArithmeticOperator(tok lexer.Token, left Expression, right Expression) (*ArithmeticOp, error) {
	typ := lexer.MaxType(left.Type(), right.Type())
	if typ == nil {
		return nil, errors.New("Type Error")
	}
	return &ArithmeticOp{
		op:    tok,
		typ:   typ,
		left:  left,
		right: right,
	}, nil
}

func (ao *ArithmeticOp) Op() lexer.Token {
	return ao.op
}

func (ao *ArithmeticOp) Type() lexer.Type {
	return ao.typ
}

func (ao *ArithmeticOp) Generate(b *strings.Builder) (Expression, error) {
	lr, err := ao.left.Reduce(b)
	if err != nil {
		return nil, err
	}
	rr, err := ao.right.Reduce(b)
	if err != nil {
		return nil, err
	}
	n, err := NewArithmeticOperator(ao.op, lr, rr)
	if err != nil {
		return nil, err
	}
	return n, nil
}

func (ao *ArithmeticOp) Reduce(b *strings.Builder) (Expression, error) {
	x, err := ao.Generate(b)
	if err != nil {
		return nil, err
	}
	tmp := NewTemp(ao.Type())
	Emit(b, fmt.Sprintf("%s = %s", tmp.String(), x.String()))
	return tmp, nil
}

func (ao *ArithmeticOp) String() string {
	return fmt.Sprintf("%s %s %s", ao.left.String(), ao.op.String(), ao.right.String())
}

func (ao *ArithmeticOp) Jumps(b *strings.Builder, to int, from int) error {
	EmitJumps(b, ao.String(), to, from)
	return nil
}

type UnaryOp struct {
	op   lexer.Token
	typ  lexer.Type
	rest Expression
}

func NewUnaryOp(tok lexer.Token, rest Expression) (*UnaryOp, error) {
	typ := lexer.MaxType(lexer.IntType(), rest.Type())
	if typ == nil {
		return nil, errors.New("Type Error")
	}
	return &UnaryOp{
		op:   tok,
		typ:  typ,
		rest: rest,
	}, nil
}

func (u *UnaryOp) Op() lexer.Token {
	return u.op
}

func (u *UnaryOp) Type() lexer.Type {
	return u.typ
}

func (u *UnaryOp) Generate(b *strings.Builder) (Expression, error) {
	r, err := u.rest.Reduce(b)
	if err != nil {
		return nil, err
	}
	return NewUnaryOp(u.op, r)
}

func (u *UnaryOp) Reduce(b *strings.Builder) (Expression, error) {
	x, err := u.Generate(b)
	if err != nil {
		return nil, err
	}
	tmp := NewTemp(u.Type())
	Emit(b, fmt.Sprintf("%s = %s", tmp.String(), x.String()))
	return tmp, nil
}

func (u *UnaryOp) String() string {
	return fmt.Sprintf("%s %s", u.op.String(), u.rest.String())
}

func (u *UnaryOp) Jumps(b *strings.Builder, to int, from int) error {
	EmitJumps(b, u.String(), to, from)
	return nil
}

type AccessOp struct {
	Array *Identifier
	Index Expression
	typ   lexer.Type
}

func (ao *AccessOp) Op() lexer.Token {
	return lexer.AccessWord()
}

func (ao *AccessOp) Type() lexer.Type {
	return ao.typ
}

func (ao *AccessOp) Generate(b *strings.Builder) (Expression, error) {
	idx, err := ao.Index.Reduce(b)
	if err != nil {
		return nil, err
	}
	return &AccessOp{
		Array: ao.Array,
		Index: idx,
		typ:   ao.typ,
	}, nil
}

func (ao *AccessOp) Reduce(b *strings.Builder) (Expression, error) {
	x, err := ao.Generate(b)
	if err != nil {
		return nil, err
	}
	tmp := NewTemp(ao.Type())
	Emit(b, fmt.Sprintf("%s = %s", tmp.String(), x.String()))
	return tmp, nil
}

func (ao *AccessOp) String() string {
	return fmt.Sprintf("%s [%s]", ao.Array.String(), ao.Index.String())
}

func (ao *AccessOp) Jumps(b *strings.Builder, to int, from int) error {
	ra, err := ao.Reduce(b)
	if err != nil {
		return err
	}
	EmitJumps(b, ra.String(), to, from)
	return nil
}

func check(tleft lexer.Type, tright lexer.Type) lexer.Type {
	if tleft == lexer.BoolType() && tright == lexer.BoolType() {
		return lexer.BoolType()
	}
	return nil
}

type NotLogicOp struct {
	op   lexer.Token
	expr Expression
}

func NewNotLogicOp(tok lexer.Token, expr Expression) (*NotLogicOp, error) {
	if expr.Type() != lexer.BoolType() {
		return nil, errors.New("Type Error")
	}
	if tok.Tag() != lexer.Tag('!') {
		return nil, errors.New("Lexer Error")
	}
	return &NotLogicOp{
		expr: expr,
	}, nil
}

func (n *NotLogicOp) Op() lexer.Token {
	return n.op
}

func (n *NotLogicOp) Type() lexer.Type {
	return lexer.BoolType()
}

func (n *NotLogicOp) Generate(b *strings.Builder) (Expression, error) {
	from := NewLabel()
	a := NewLabel()
	tmp := NewTemp(n.Type())
	if err := n.Jumps(b, 0, from); err != nil {
		return nil, err
	}
	Emit(b, fmt.Sprintf("%s = true", tmp.String()))

	Emit(b, fmt.Sprintf("goto L%d", a))
	EmitLabel(b, from)
	Emit(b, fmt.Sprintf("%s = false", tmp.String()))
	EmitLabel(b, a)
	return tmp, nil
}

func (n *NotLogicOp) Reduce(*strings.Builder) (Expression, error) {
	return n, nil
}

func (n *NotLogicOp) String() string {
	return fmt.Sprintf("%s %s", n.op.String(), n.expr.String())
}

func (n *NotLogicOp) Jumps(b *strings.Builder, to int, from int) error {
	return n.expr.Jumps(b, from, to)
}

type OrLogicOp struct {
	left  Expression
	right Expression
}

func NewOrLogicOp(left Expression, right Expression) (*OrLogicOp, error) {
	if check(left.Type(), right.Type()) != lexer.BoolType() {
		return nil, errors.New("Type Error")
	}
	return &OrLogicOp{
		left:  left,
		right: right,
	}, nil
}

func (o *OrLogicOp) Op() lexer.Token {
	return lexer.OrWord()
}

func (o *OrLogicOp) Type() lexer.Type {
	return lexer.BoolType()
}

func (o *OrLogicOp) Generate(b *strings.Builder) (Expression, error) {
	from := NewLabel()
	a := NewLabel()
	tmp := NewTemp(o.Type())
	if err := o.Jumps(b, 0, from); err != nil {
		return nil, err
	}
	Emit(b, fmt.Sprintf("%s = true", tmp.String()))
	Emit(b, fmt.Sprintf("goto L%d", a))
	EmitLabel(b, from)
	Emit(b, fmt.Sprintf("%s = false", tmp.String()))
	EmitLabel(b, a)
	return tmp, nil
}

func (o *OrLogicOp) Reduce(*strings.Builder) (Expression, error) {
	return o, nil
}

func (o *OrLogicOp) String() string {
	return fmt.Sprintf("%s || %s", o.left.String(), o.right.String())
}

func (o *OrLogicOp) Jumps(b *strings.Builder, to int, from int) error {
	label := to
	if to == 0 {
		label = NewLabel()
	}
	if err := o.left.Jumps(b, label, 0); err != nil {
		return err
	}
	if err := o.right.Jumps(b, to, from); err != nil {
		return err
	}
	if to == 0 {
		EmitLabel(b, label)
	}
	return nil
}

type AndLogicOp struct {
	left  Expression
	right Expression
}

func NewAndLogicOp(left Expression, right Expression) (*AndLogicOp, error) {
	if check(left.Type(), right.Type()) != lexer.BoolType() {
		return nil, errors.New("Type Error")
	}
	return &AndLogicOp{
		left:  left,
		right: right,
	}, nil
}

func (a *AndLogicOp) Op() lexer.Token {
	return lexer.AndWord()
}

func (a *AndLogicOp) Type() lexer.Type {
	return lexer.BoolType()
}

func (a *AndLogicOp) Generate(b *strings.Builder) (Expression, error) {
	from := NewLabel()
	label := NewLabel()
	tmp := NewTemp(a.Type())
	if err := a.Jumps(b, 0, from); err != nil {
		return nil, err
	}
	Emit(b, fmt.Sprintf("%s = true", tmp.String()))
	Emit(b, fmt.Sprintf("goto L%d", label))
	EmitLabel(b, from)
	Emit(b, fmt.Sprintf("%s = false", tmp.String()))
	EmitLabel(b, label)
	return tmp, nil
}

func (a *AndLogicOp) Reduce(*strings.Builder) (Expression, error) {
	return a, nil
}

func (a *AndLogicOp) String() string {
	return fmt.Sprintf("%s && %s", a.left.String(), a.right.String())
}

func (a *AndLogicOp) Jumps(b *strings.Builder, to int, from int) error {
	label := from
	if from == 0 {
		label = NewLabel()
	}
	if err := a.left.Jumps(b, 0, label); err != nil {
		return err
	}
	if err := a.right.Jumps(b, to, from); err != nil {
		return err
	}
	if from == 0 {
		EmitLabel(b, label)
	}
	return nil
}
