from .inter import Node

class Statement(Node):
  def gen(self, begin: int, after: int) -> str:
    return ""

NULL_STMT = Statement()
enclosing: Statement = NULL_STMT
