#include <gtest/gtest.h>

#include "cpp/parser/parser.hpp"

#include "cpp/lexer/lexer.hpp"

#include "cpp/inter/node.hpp"
#include "cpp/inter/temporary.hpp"

#include <string>
#include <sstream>
#include <vector>


using inter::Node;
using inter::Temporary;

struct TestCase {
  std::string source;
  std::string gen;
};

TEST(TestParser, ParserTests) {
  std::vector<TestCase> test_cases{
    TestCase{"{}", "L1:L2:"},
    TestCase{"{int i;}", "L1:L2:"},
    TestCase{"{int i;float f;bool[100] b;}", "L1:L2:"},
    TestCase{"{int i; i = 10;}", "L1:\ti = 10\nL2:"},
    TestCase{"{int i; i = i + 10;}", "L1:\ti = i + 10\nL2:"},
    TestCase{
      "{int i;int[20] arr; i = 10; arr[i] = 10;}",
      R"#(L1:	i = 10
L3:	t1 = i * 4
	arr[ t1 ] = 10
L2:)#",
    },
    TestCase{
      "{int i; int j; bool a; i = i + 10; j = 11; a = i == j;}",
      R"#(L1:	i = i + 10
L3:	j = 11
L4:	iffalse i == j goto L5
	t1 = true
	goto L6
L5:	t1 = false
L6:	a = t1
L2:)#",
    },
    TestCase{
      "{int i; int j; j = 12; while (i > j) i = i + 1;}",
      R"#(L1:	j = 12
L3:	iffalse i > j goto L2
L4:	i = i + 1
	goto L3
L2:)#"
    },
    TestCase{
      "{int i; int j; j = 12; do i = i + 1; while (i > j);}",
      R"#(L1:	j = 12
L3:	i = i + 1
L4:	if i > j goto L3
L2:)#"
    },
    TestCase{
      "{ while (true) { break; } }",
      R"#(L1:L3:	goto L2
	goto L1
L2:)#"
    },
    TestCase{
      "{int i; int j; i = 10; j = 1; while (j < i) { i = i + 1; break;} }",
      R"#(L1:	i = 10
L3:	j = 1
L4:	iffalse j < i goto L2
L5:	i = i + 1
L6:	goto L2
	goto L4
L2:)#"
    },
    TestCase{
      "{int i; int j; while (true) i = i + 1;}",
      R"#(L1:L3:	i = i + 1
	goto L1
L2:)#"
    },
    TestCase{
      "{int i; int j; i = 10; j = 1; while (j < i) { i = i + 1; break;} }",
      R"#(L1:	i = 10
L3:	j = 1
L4:	iffalse j < i goto L2
L5:	i = i + 1
L6:	goto L2
	goto L4
L2:)#"
    },
    TestCase{
      R"#({
        int i; int j; float v; float x; float[100] a;
        while (true) {
          do i = i + 1; while (a[i] < v);
          do j = j - 1; while (a[j] > v);
          if (i >= j) break;
          x = a[i];
          a[i] = a[j];
          a[j] = x;
        }
      })#",
      R"#(L1:L3:	i = i + 1
L5:	t1 = i * 8
	t2 = a[ t1 ]
	if t2 < v goto L3
L4:	j = j - 1
L7:	t3 = j * 8
	t4 = a[ t3 ]
	if t4 > v goto L4
L6:	iffalse i >= j goto L8
L9:	goto L2
L8:	t5 = i * 8
	x = a[ t5 ]
L10:	t6 = i * 8
	t7 = j * 8
	t8 = a[ t7 ]
	a[ t6 ] = t8
L11:	t9 = j * 8
	a[ t9 ] = x
	goto L1
L2:)#"
    },
  };

  for (auto &tc : test_cases) {
    Node::reset_labels();
    Temporary::reset_temp_count();
    std::stringstream source(tc.source);
    auto lex = lexer::Lexer::create(source);

    parser::Parser parser(lex);
    ASSERT_EQ(parser.program(), tc.gen);
  }
}
