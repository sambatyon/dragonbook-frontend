use std::fmt;

pub trait Statement: fmt::Display {
  fn generate(&mut self, b: &mut String, being: i64, after: i64) -> Result<(), String>;
  fn after(&self) -> i64;
}

pub struct NullStmt {}

impl NullStmt {
  pub fn new() -> NullStmt {
    NullStmt { }
  }
}

impl Statement for NullStmt {
  fn generate(&mut self, b: &mut String, being: i64, after: i64) -> Result<(), String> {
      Ok(())
  }

  fn after(&self) -> i64 {
      0
  }
}

impl fmt::Display for NullStmt {
  fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
    write!(f, "")
  }
}
