use std::collections::HashMap;
use inter::expr;

pub struct Environment {
  table: HashMap<String, expr::Identifier>,
  previous: Box<Option<Environment>>,
}

impl Environment {
  fn new() -> Box<Environment> {
    Box::new(Environment { table: HashMap::new(), previous: Box::new(None) })
  }
}
