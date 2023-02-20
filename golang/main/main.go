package main

import (
	"bufio"
	"dragonbook/lexer"
	"dragonbook/parser"
	"fmt"
	"os"
	"strings"
)

func main() {
	reader := bufio.NewReader(os.Stdin)
	lex := lexer.NewLexer(reader)
	parser, err := parser.NewParser(lex)
	if err != nil {
		fmt.Printf("Could not create parser: %s\n", err)
	}
	var b strings.Builder
	if err := parser.Program(&b); err != nil {
		fmt.Printf("Error parsing program: %s", err)
	}

	fmt.Println(b.String())
}
