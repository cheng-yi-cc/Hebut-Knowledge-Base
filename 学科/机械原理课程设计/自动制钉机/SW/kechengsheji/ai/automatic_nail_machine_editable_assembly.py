from pathlib import Path
import sys

sys.path.insert(0, str(Path(__file__).resolve().parents[1]))

from automatic_nail_machine import gen_step as _gen_full_assembly


def gen_step():
    return _gen_full_assembly()
