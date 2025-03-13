package ast

import (
	"fmt"
	"strings"
)

var labels int = 0

func ResetLabels() {
	labels = 0
}

func NewLabel() int {
	labels++
	return labels
}

func EmitLabel(b *strings.Builder, i int) {
	b.WriteString(fmt.Sprintf("L%d:", i))
}

func Emit(b *strings.Builder, s string) {
	b.WriteString(fmt.Sprintf("\t%s\n", s))
}
