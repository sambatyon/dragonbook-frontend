package parser

import (
	"dragonbook/inter"
	"dragonbook/lexer"
	"errors"
	"fmt"
	"strings"
)

type environment struct {
	table    map[string]*inter.Identifier
	previous *environment
}

func NewEnvironment(top *environment) *environment {
	return &environment{make(map[string]*inter.Identifier), top}
}

func (e *environment) put(key string, value *inter.Identifier) {
	e.table[key] = value
}

func (e *environment) get(key string) (*inter.Identifier, bool) {
	for cur := e; cur != nil; cur = e.previous {
		found, ok := e.table[key]
		if ok {
			return found, true
		}
	}
	return nil, false
}

type Parser struct {
	lex       *lexer.Lexer
	lookahead lexer.Token
	top       *environment
	used      int
	encstmt   inter.Statement
}

func NewParser(lex *lexer.Lexer) (*Parser, error) {
	if lex == nil {
		return nil, errors.New("lexer is nil")
	}
	res := &Parser{lex, nil, nil, 0, inter.NullStmt()}
	if err := res.move(); err != nil {
		return nil, err
	}
	return res, nil
}

func (p *Parser) move() error {
	la, err := p.lex.Scan()
	if err != nil {
		return errors.New(fmt.Sprintf("%v near line %d", err, p.lex.Line))
	}
	p.lookahead = la
	return nil
}

func (p *Parser) match(t lexer.Tag) error {
	if p.lookahead.Tag() != t {
		return errors.New(fmt.Sprintf("Syntax error near line %d", p.lex.Line))
	}
	p.move()
	return nil
}

func (p *Parser) Program(b *strings.Builder) error {
	stmt, err := p.block()
	if err != nil {
		return err
	}
	begin := inter.NewLabel()
	after := inter.NewLabel()
	inter.EmitLabel(b, begin)
	if err := stmt.Generate(b, begin, after); err != nil {
		return err
	}
	inter.EmitLabel(b, after)
	return nil
}

func (p *Parser) block() (inter.Statement, error) {
	if err := p.match(lexer.Tag('{')); err != nil {
		return nil, err
	}
	saved := p.top
	p.top = NewEnvironment(p.top)
	if err := p.decls(); err != nil {
		return nil, err
	}
	stmts, err := p.stmts()
	if err != nil {
		return nil, err
	}
	p.top = saved
	return stmts, nil
}

func (p *Parser) decls() error {
	for p.lookahead.Tag() == lexer.BASIC {
		t, err := p.typ()
		if err != nil {
			return err
		}
		tok := p.lookahead
		if err := p.match(lexer.ID); err != nil {
			return err
		}
		if err := p.match(lexer.Tag(';')); err != nil {
			return err
		}
		id := inter.NewIdentifier(tok, t, p.used)
		p.top.put(tok.String(), id)
		p.used += t.Width()
	}
	return nil
}

func (p *Parser) typ() (lexer.Type, error) {
	t, ok := p.lookahead.(lexer.Type)
	if !ok {
		return nil, errors.New(fmt.Sprintf("Syntax error (expected type) near line %d", p.lex.Line))
	}
	if err := p.match(lexer.BASIC); err != nil {
		return nil, err
	}
	if p.lookahead.Tag() != lexer.Tag('[') {
		return t, nil
	}
	return p.dims(t)
}

func (p *Parser) dims(of lexer.Type) (lexer.Type, error) {
	p.match(lexer.Tag('['))
	tok := p.lookahead
	p.match(lexer.INTEGER)
	sz, ok := tok.(*lexer.Integer)
	if !ok {
		return nil, errors.New(fmt.Sprintf("Syntax error near line %d", p.lex.Line))
	}
	p.match(lexer.Tag(']'))

	if p.lookahead.Tag() == lexer.Tag('[') {
		var err error
		of, err = p.dims(of)
		if err != nil {
			return nil, err
		}
	}
	return &lexer.Array{Of: of, Length: int(sz.Value)}, nil
}

func (p *Parser) stmts() (inter.Statement, error) {
	if p.lookahead.Tag() == lexer.Tag('}') {
		return inter.NullStmt(), nil
	}
	head, err := p.stmt()
	if err != nil {
		return nil, err
	}
	tail, err := p.stmts()
	if err != nil {
		return nil, err
	}
	return inter.NewStmtSeq(head, tail), nil
}

func (p *Parser) stmt() (inter.Statement, error) {
	switch p.lookahead.Tag() {
	case lexer.Tag(';'):
		if err := p.move(); err != nil {
			return nil, err
		}
		return inter.NullStmt(), nil

	case lexer.IF:
		if err := p.match(lexer.IF); err != nil {
			return nil, err
		}
		if err := p.match(lexer.Tag('(')); err != nil {
			return nil, err
		}
		expr, err := p.bool()
		if err != nil {
			return nil, err
		}
		if err := p.match(lexer.Tag(')')); err != nil {
			return nil, err
		}
		body, err := p.stmt()
		if err != nil {
			return nil, err
		}
		if p.lookahead.Tag() != lexer.ELSE {
			return inter.NewIfStmt(expr, body)
		}
		if err := p.match(lexer.ELSE); err != nil {
			return nil, err
		}
		els, err := p.stmt()
		if err != nil {
			return nil, err
		}
		return inter.NewElseStmt(expr, body, els)

	case lexer.WHILE:
		saved := p.encstmt
		while := &inter.WhileStmt{nil, nil, 0}
		p.encstmt = while
		if err := p.match(lexer.WHILE); err != nil {
			return nil, err
		}
		if err := p.match(lexer.Tag('(')); err != nil {
			return nil, err
		}

		expr, err := p.bool()
		if err != nil {
			return nil, err
		}
		if expr.Type() != lexer.BoolType() {
			return nil, errors.New("Boolean Required in while loop")
		}
		while.Cond = expr

		if err := p.match(lexer.Tag(')')); err != nil {
			return nil, err
		}
		body, err := p.stmt()
		if err != nil {
			return nil, err
		}
		while.Body = body
		p.encstmt = saved
		return while, nil

	case lexer.DO:
		saved := p.encstmt
		do := &inter.DoStmt{nil, nil, 0}
		p.encstmt = do
		if err := p.match(lexer.DO); err != nil {
			return nil, err
		}
		body, err := p.stmt()
		if err != nil {
			return nil, err
		}
		do.Body = body

		if err := p.match(lexer.WHILE); err != nil {
			return nil, err
		}
		if err := p.match(lexer.Tag('(')); err != nil {
			return nil, err
		}
		expr, err := p.bool()
		if err != nil {
			return nil, err
		}
		if expr.Type() != lexer.BoolType() {
			return nil, errors.New("Boolean Required in while loop")
		}
		do.Cond = expr
		if err := p.match(lexer.Tag(')')); err != nil {
			return nil, err
		}
		if err := p.match(lexer.Tag(';')); err != nil {
			return nil, err
		}
		p.encstmt = saved
		return do, nil

	case lexer.BREAK:
		if err := p.match(lexer.BREAK); err != nil {
			return nil, err
		}
		if err := p.match(lexer.Tag(';')); err != nil {
			return nil, err
		}
		return inter.NewBreakStmt(p.encstmt)

	case lexer.Tag('{'):
		return p.block()

	default:
		return p.assign()
	}
}

func (p *Parser) assign() (inter.Statement, error) {
	tok := p.lookahead
	if err := p.match(lexer.ID); err != nil {
		return nil, err
	}
	id, ok := p.top.get(tok.String())
	if !ok {
		return nil, errors.New(fmt.Sprintf("%s undeclared", tok.String()))
	}
	var stmt inter.Statement
	if p.lookahead.Tag() == lexer.Tag('=') {
		if err := p.move(); err != nil {
			return nil, err
		}
		expr, err := p.bool()
		if err != nil {
			return nil, err
		}
		stmt, err = inter.NewAssignStmt(id, expr)
		if err != nil {
			return nil, err
		}
	} else {
		access, err := p.offset(id)
		if err != nil {
			return nil, err
		}
		if err := p.match(lexer.Tag('=')); err != nil {
			return nil, err
		}
		expr, err := p.bool()
		if err != nil {
			return nil, err
		}
		stmt, err = inter.NewAssignArrayStmt(access, expr)
		if err != nil {
			return nil, err
		}
	}
	if err := p.match(lexer.Tag(';')); err != nil {
		return nil, err
	}
	return stmt, nil
}

func (p *Parser) bool() (inter.Expression, error) {
	expr, err := p.join()
	if err != nil {
		return nil, err
	}
	for p.lookahead.Tag() == lexer.OR {
		if err := p.move(); err != nil {
			return nil, err
		}
		right, err := p.join()
		if err != nil {
			return nil, err
		}
		expr, err = inter.NewOrLogicOp(expr, right)
		if err != nil {
			return nil, err
		}
	}
	return expr, nil
}

func (p *Parser) join() (inter.Expression, error) {
	expr, err := p.equality()
	if err != nil {
		return nil, err
	}
	for p.lookahead.Tag() == lexer.AND {
		if err := p.move(); err != nil {
			return nil, err
		}
		right, err := p.equality()
		if err != nil {
			return nil, err
		}
		expr, err = inter.NewAndLogicOp(expr, right)
		if err != nil {
			return nil, err
		}
	}
	return expr, nil
}

func (p *Parser) equality() (inter.Expression, error) {
	expr, err := p.relation()
	if err != nil {
		return nil, err
	}
	for p.lookahead.Tag() == lexer.EQ || p.lookahead.Tag() == lexer.NE {
		tok := p.lookahead
		if err = p.move(); err != nil {
			return nil, err
		}
		right, err := p.relation()
		if err != nil {
			return nil, err
		}
		expr, err = inter.NewRelationOp(tok, expr, right)
		if err != nil {
			return nil, err
		}
	}
	return expr, nil
}

func (p *Parser) relation() (inter.Expression, error) {
	expr, err := p.expr()
	if err != nil {
		return nil, err
	}
	switch p.lookahead.Tag() {
	case lexer.LE, lexer.GE, lexer.Tag('<'), lexer.Tag('>'):
		tok := p.lookahead
		if err = p.move(); err != nil {
			return nil, err
		}
		right, err := p.expr()
		if err != nil {
			return nil, err
		}
		return inter.NewRelationOp(tok, expr, right)
	default:
		return expr, nil
	}
}

func (p *Parser) expr() (inter.Expression, error) {
	expr, err := p.term()
	if err != nil {
		return nil, err
	}
	for p.lookahead.Tag() == lexer.Tag('+') || p.lookahead.Tag() == lexer.Tag('-') {
		tok := p.lookahead
		if err := p.move(); err != nil {
			return nil, err
		}
		right, err := p.term()
		if err != nil {
			return nil, err
		}
		expr, err = inter.NewArithmeticOperator(tok, expr, right)
		if err != nil {
			return nil, err
		}
	}
	return expr, nil
}

func (p *Parser) term() (inter.Expression, error) {
	expr, err := p.unary()
	if err != nil {
		return nil, err
	}
	for p.lookahead.Tag() == lexer.Tag('*') || p.lookahead.Tag() == lexer.Tag('/') {
		tok := p.lookahead
		if err := p.move(); err != nil {
			return nil, err
		}
		right, err := p.unary()
		if err != nil {
			return nil, err
		}
		expr, err = inter.NewArithmeticOperator(tok, expr, right)
		if err != nil {
			return nil, err
		}
	}
	return expr, nil
}

func (p *Parser) unary() (inter.Expression, error) {
	if p.lookahead.Tag() == lexer.Tag('-') {
		if err := p.move(); err != nil {
			return nil, err
		}
		expr, err := p.unary()
		if err != nil {
			return nil, err
		}
		return inter.NewUnaryOp(lexer.MinusWord(), expr)
	}
	if p.lookahead.Tag() == lexer.Tag('!') {
		tok := p.lookahead
		if err := p.move(); err != nil {
			return nil, err
		}
		expr, err := p.unary()
		if err != nil {
			return nil, err
		}
		return inter.NewNotLogicOp(tok, expr)
	}
	return p.factor()
}

func (p *Parser) factor() (inter.Expression, error) {
	switch p.lookahead.Tag() {
	case lexer.Tag('('):
		if err := p.move(); err != nil {
			return nil, err
		}
		expr, err := p.bool()
		if err != nil {
			return nil, err
		}
		err = p.match(')')
		if err != nil {
			return nil, err
		}
		return expr, nil

	case lexer.INTEGER:
		expr, err := inter.NewIntConstant(p.lookahead)
		if err != nil {
			return nil, err
		}
		if err := p.move(); err != nil {
			return nil, err
		}
		return expr, nil

	case lexer.REAL:
		expr, err := inter.NewFloatConstant(p.lookahead)
		if err != nil {
			return nil, err
		}
		if err := p.move(); err != nil {
			return nil, err
		}
		return expr, nil

	case lexer.TRUE:
		expr := inter.TrueConstant()
		if err := p.move(); err != nil {
			return nil, err
		}
		return expr, nil

	case lexer.FALSE:
		expr := inter.FalseConstant()
		if err := p.move(); err != nil {
			return nil, err
		}
		return expr, nil

	case lexer.ID:
		id, ok := p.top.get(p.lookahead.String())
		if !ok {
			return nil, errors.New(fmt.Sprintf("Undeclared identifier %s", p.lookahead.String()))
		}
		if err := p.move(); err != nil {
			return nil, err
		}
		if p.lookahead.Tag() != lexer.Tag('[') {
			return id, nil
		}
		return p.offset(id)

	default:
		return nil, errors.New("Syntax Error")
	}
}

func (p *Parser) offset(id *inter.Identifier) (*inter.AccessOp, error) {
	typ := id.Type()
	if err := p.match(lexer.Tag('[')); err != nil {
		return nil, err
	}
	index, err := p.bool()
	if err != nil {
		return nil, err
	}
	arr, ok := typ.(*lexer.Array)
	if !ok {
		return nil, errors.New("Type error")
	}
	typ = arr.Of
	width, err := inter.NewIntConstant(&lexer.Integer{int64(typ.Width())})
	if err != nil {
		return nil, err
	}
	t1, err := inter.NewArithmeticOperator(lexer.NewToken(lexer.Tag('*')), index, width)
	if err != nil {
		return nil, err
	}
	loc := t1

	for p.lookahead.Tag() == lexer.Tag('[') {
		if err := p.match(lexer.Tag('[')); err != nil {
			return nil, err
		}
		index, err = p.bool()
		if err != nil {
			return nil, err
		}
		arr, ok = typ.(*lexer.Array)
		if !ok {
			return nil, errors.New("Type error")
		}
		typ = arr.Of
		width, err = inter.NewIntConstant(&lexer.Integer{int64(typ.Width())})
		if err != nil {
			return nil, err
		}
		t1, err = inter.NewArithmeticOperator(lexer.NewToken(lexer.Tag('*')), index, width)
		if err != nil {
			return nil, err
		}
		t2, err := inter.NewArithmeticOperator(lexer.NewToken(lexer.Tag('+')), loc, t1)
		if err != nil {
			return nil, err
		}
		loc = t2
	}
	return inter.NewAccessOp(id, loc, typ)
}
