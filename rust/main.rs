use std::io::BufReader;

use lexer::Lexer;
use parser::Parser;

fn main() {
  let lexer = Lexer::new(BufReader::new(std::io::stdin()));
  let mut parser = Parser::new(lexer).expect("Creating parser");

  let mut str = String::new();
  parser.program(&mut str).expect("Parsing program");
  println!("{}", str);
}
