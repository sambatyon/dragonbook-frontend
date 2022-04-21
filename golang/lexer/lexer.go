package lexer

import (
	"bufio"
	"io"
	"strings"
	"unicode"
)

type Lexer struct {
	Line   int
	peek   rune
	words  map[string]Token
	reader *bufio.Reader
}

func NewLexer(rd io.Reader) *Lexer {
	ret := &Lexer{
		Line:   1,
		peek:   ' ',
		words:  make(map[string]Token),
		reader: bufio.NewReader(rd),
	}

	ret.words["if"] = &Word{tag: IF, lexeme: "if"}
	ret.words["else"] = &Word{tag: ELSE, lexeme: "else"}
	ret.words["while"] = &Word{tag: WHILE, lexeme: "while"}
	ret.words["do"] = &Word{tag: DO, lexeme: "do"}
	ret.words["break"] = &Word{tag: BREAK, lexeme: "break"}
	ret.words["true"] = TrueWord()
	ret.words["false"] = FalseWord()
	ret.words["int"] = IntType()
	ret.words["float"] = FloatType()
	ret.words["char"] = CharType()
	ret.words["bool"] = BoolType()

	return ret
}

func (l *Lexer) read() error {
	b, err := l.reader.ReadByte()
	if err != nil {
		if err == io.EOF {
			l.peek = rune(0)
			return nil
		}
		return err
	}
	l.peek = rune(b)
	return nil
}

func (l *Lexer) readCh(c rune) (bool, error) {
	err := l.read()
	if err != nil {
		return false, err
	}
	if l.peek != c {
		return false, nil
	}
	l.peek = ' '
	return true, nil
}

func (l *Lexer) Scan() (Token, error) {
	if l.peek != ' ' {
		tok := &Tok{tag: Tag(l.peek)}
		l.peek = ' '
		return tok, nil
	}
	if l.peek == rune(0) {
		return Eof(), nil
	}
	for {
		err := l.read()
		if err != nil {
			return nil, err
		}
		if l.peek == ' ' || l.peek == '\t' || l.peek == '\r' {
			continue
		}
		if l.peek == '\n' {
			l.Line++
			continue
		}
		break
	}

	var b bool
	var err error
	switch l.peek {
	case '&':
		b, err = l.readCh('&')
		if err != nil && err != io.EOF {
			return nil, err
		}
		if b {
			return AndWord(), nil
		}
		return &Tok{tag: Tag('&')}, nil
	case '|':
		b, err = l.readCh('|')
		if err != nil && err != io.EOF {
			return nil, err
		}
		if b {
			return OrWord(), nil
		}
		return &Tok{tag: Tag('|')}, nil
	case '=':
		b, err = l.readCh('=')
		if err != nil && err != io.EOF {
			return nil, err
		}
		if b {
			return EqWord(), nil
		}
		return &Tok{tag: Tag('=')}, nil
	case '!':
		b, err = l.readCh('=')
		if err != nil && err != io.EOF {
			return nil, err
		}
		if b {
			return NeWord(), nil
		}
		return &Tok{tag: Tag('!')}, nil
	case '<':
		b, err = l.readCh('=')
		if err != nil && err != io.EOF {
			return nil, err
		}
		if b {
			return LeWord(), nil
		}
		return &Tok{tag: Tag('<')}, nil
	case '>':
		b, err = l.readCh('=')
		if err != nil && err != io.EOF {
			return nil, err
		}
		if b {
			return GeWord(), nil
		}
		return &Tok{tag: Tag('>')}, nil
	}

	if unicode.IsDigit(l.peek) {
		var v int64
		for {
			v = 10*v + int64(l.peek-'0')
			if err = l.read(); err != nil {
				if err == io.EOF {
					break
				}
				return nil, err
			}
			if !unicode.IsDigit(l.peek) {
				break
			}
		}

		if l.peek != '.' {
			return &Integer{Value: v}, nil
		}

		x := float64(v)
		var d float64 = 10.0
		for {
			if err = l.read(); err != nil {
				if err == io.EOF {
					break
				}
				return nil, err
			}
			if !unicode.IsDigit(l.peek) {
				break
			}
			x += float64(int64(l.peek-'0')) / d
			d *= 10.0
		}
		return &Real{Value: x}, nil
	}

	if unicode.IsLetter(l.peek) {
		var sb strings.Builder
		for {
			sb.WriteRune(l.peek)
			if err = l.read(); err != nil {
				if err == io.EOF {
					break
				}
				return nil, err
			}
			if !unicode.IsLetter(l.peek) && !unicode.IsDigit(l.peek) {
				break
			}
		}
		s := sb.String()
		if w, ok := l.words[s]; ok {
			return w, nil
		}

		w := &Word{tag: ID, lexeme: s}
		l.words[s] = w
		return w, nil
	}

	tok := &Tok{tag: Tag(l.peek)}
	l.peek = ' '
	return tok, nil
}
