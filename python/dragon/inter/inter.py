import threading

def __label_init() -> threading.local:
  l = threading.local()
  l.count = 0
  return l

__l: threading.local = __label_init()

def reset_labels() -> None:
  __l.count = 0

def new_label() -> int:
  lbl: int = __l.count
  __l.count += 1
  return lbl

def emit_label(i: int) -> str:
  return f"L{i}:"

def emit(s: str) -> str:
  return f"\t{s}\n"


class Node:
  def error(self, msg: str, line: int|None) -> None:
    if line is None:
      raise Exception(msg)
    else:
      raise Exception(f"near line {line}: {msg}")
