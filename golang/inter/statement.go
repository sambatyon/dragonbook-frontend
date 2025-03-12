package inter

import (
	"dragonbook/lexer"
	"errors"
	"fmt"
	"strings"
)

type Statement interface {
	Generate(b *strings.Builder, begin int, after int) error
	after() int
}

type nullStmt struct{}

func (n *nullStmt) Generate(*strings.Builder, int, int) error {
	return nil
}

func (n *nullStmt) after() int {
	return 0
}

var null = &nullStmt{}

func NullStmt() Statement {
	return null
}

type AssignStmt struct {
	id   *Identifier
	expr Expression
}

func NewAssignStmt(id *Identifier, expr Expression) (*AssignStmt, error) {
	if (id.Type().Numeric() && !expr.Type().Numeric()) || (!id.Type().Numeric() && expr.Type().Numeric()) {
		return nil, errors.New("Type Error")
	}
	if (id.Type() == lexer.BoolType() && expr.Type() != lexer.BoolType()) || (id.Type() != lexer.BoolType() && expr.Type() == lexer.BoolType()) {
		return nil, errors.New("Type Error")
	}
	return &AssignStmt{id, expr}, nil
}

func (a *AssignStmt) Generate(b *strings.Builder, begin int, after int) error {
	expr, err := a.expr.Generate(b)
	if err != nil {
		return err
	}
	Emit(b, fmt.Sprintf("%s = %s", a.id.String(), expr.String()))
	return nil
}

func (a *AssignStmt) after() int {
	return 0
}

type AssignArrayStmt struct {
	id    *Identifier
	index Expression
	expr  Expression
}

func NewAssignArrayStmt(access *AccessOp, expr Expression) (*AssignArrayStmt, error) {
	_, aok := access.Type().(*lexer.Array)
	_, eok := expr.Type().(*lexer.Array)
	if (aok || eok) || (access.Type() != expr.Type()) {
		return nil, errors.New("Type error")
	}
	if (access.Type().Numeric() && !expr.Type().Numeric()) || (!access.Type().Numeric() && expr.Type().Numeric()) {
		return nil, errors.New("Type error")
	}

	return &AssignArrayStmt{id: access.Array, index: access.Index, expr: expr}, nil
}

func (aa *AssignArrayStmt) Generate(b *strings.Builder, begin int, after int) error {
	idx, err := aa.index.Reduce(b)
	if err != nil {
		return err
	}
	ex, err := aa.expr.Reduce(b)
	if err != nil {
		return err
	}
	Emit(b, fmt.Sprintf("%s [ %s ] = %s", aa.id.String(), idx.String(), ex.String()))
	return nil
}

func (aa *AssignArrayStmt) after() int {
	return 0
}

type StmtSeq struct {
	head Statement
	tail Statement
}

func NewStmtSeq(head Statement, tail Statement) *StmtSeq {
	return &StmtSeq{head, tail}
}

func (s *StmtSeq) Generate(b *strings.Builder, begin int, after int) error {
	if s.head == NullStmt() {
		return s.tail.Generate(b, begin, after)
	}
	if s.tail == NullStmt() {
		return s.head.Generate(b, begin, after)
	}
	label := NewLabel()
	if err := s.head.Generate(b, begin, label); err != nil {
		return err
	}
	EmitLabel(b, label)
	if err := s.tail.Generate(b, label, after); err != nil {
		return err
	}
	return nil
}

func (s *StmtSeq) after() int {
	return 0
}

type IfStmt struct {
	cond Expression
	body Statement
}

func NewIfStmt(cond Expression, body Statement) (*IfStmt, error) {
	if cond.Type() != lexer.BoolType() {
		return nil, errors.New("Boolean Required in If")
	}
	return &IfStmt{cond, body}, nil
}

func (i *IfStmt) Generate(b *strings.Builder, begin int, after int) error {
	label := NewLabel()
	if err := i.cond.Jumps(b, 0, after); err != nil {
		return err
	}
	EmitLabel(b, label)
	if err := i.body.Generate(b, label, after); err != nil {
		return err
	}
	return nil
}

func (i *IfStmt) after() int {
	return 0
}

type ElseStmt struct {
	cond   Expression
	trStmt Statement
	flStmt Statement
}

func NewElseStmt(cond Expression, tru Statement, fal Statement) (*ElseStmt, error) {
	if cond.Type() != lexer.BoolType() {
		return nil, errors.New("Boolean Required in If")
	}
	return &ElseStmt{cond, tru, fal}, nil
}

func (e *ElseStmt) Generate(b *strings.Builder, begin int, after int) error {
	label1 := NewLabel()
	label2 := NewLabel()
	if err := e.cond.Jumps(b, 0, label2); err != nil {
		return err
	}
	EmitLabel(b, label1)
	if err := e.trStmt.Generate(b, label1, after); err != nil {
		return err
	}
	Emit(b, fmt.Sprintf("goto L%d", after))
	EmitLabel(b, label2)
	if err := e.flStmt.Generate(b, label2, after); err != nil {
		return err
	}
	return nil
}

func (e *ElseStmt) after() int {
	return 0
}

type WhileStmt struct {
	Cond Expression
	Body Statement
	Aft  int
}

func NewWhileStmt(cond Expression, body Statement) (*WhileStmt, error) {
	if cond.Type() != lexer.BoolType() {
		return nil, errors.New("Boolean Required in while loop")
	}
	return &WhileStmt{cond, body, 0}, nil
}

func (w *WhileStmt) Generate(b *strings.Builder, begin int, after int) error {
	w.Aft = after
	if err := w.Cond.Jumps(b, 0, after); err != nil {
		return err
	}
	label := NewLabel()
	EmitLabel(b, label)
	if err := w.Body.Generate(b, label, begin); err != nil {
		return err
	}
	Emit(b, fmt.Sprintf("goto L%d", begin))
	return nil
}

func (w *WhileStmt) after() int {
	return w.Aft
}

type DoStmt struct {
	Cond Expression
	Body Statement
	Aft  int
}

func NewDoStmt(cond Expression, body Statement) (*DoStmt, error) {
	if cond.Type() != lexer.BoolType() {
		return nil, errors.New("Boolean Required in do loop")
	}
	return &DoStmt{cond, body, 0}, nil
}

func (d *DoStmt) Generate(b *strings.Builder, begin int, after int) error {
	d.Aft = after
	label := NewLabel()
	if err := d.Body.Generate(b, begin, label); err != nil {
		return err
	}
	EmitLabel(b, label)
	if err := d.Cond.Jumps(b, begin, 0); err != nil {
		return err
	}
	return nil
}

func (d *DoStmt) after() int {
	return d.Aft
}

type BreakStmt struct {
	enclosing Statement
}

func NewBreakStmt(enclosing Statement) (*BreakStmt, error) {
	if enclosing == NullStmt() {
		return nil, errors.New("Unenclosed break")
	}
	return &BreakStmt{enclosing: enclosing}, nil
}

func (br *BreakStmt) Generate(b *strings.Builder, begin int, after int) error {
	Emit(b, fmt.Sprintf("goto L%d", br.enclosing.after()))
	return nil
}

func (br *BreakStmt) after() int {
	return 0
}
