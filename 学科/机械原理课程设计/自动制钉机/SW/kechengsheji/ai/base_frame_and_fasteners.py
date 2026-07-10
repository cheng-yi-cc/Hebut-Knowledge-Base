from pathlib import Path
import sys

sys.path.insert(0, str(Path(__file__).resolve().parents[1]))

from automatic_nail_machine import frame_and_base


def gen_step():
    return frame_and_base()

