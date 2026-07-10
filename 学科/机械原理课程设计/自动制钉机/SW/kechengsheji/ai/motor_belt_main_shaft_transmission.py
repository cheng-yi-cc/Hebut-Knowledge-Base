from pathlib import Path
import sys

sys.path.insert(0, str(Path(__file__).resolve().parents[1]))

from automatic_nail_machine import drive_train


def gen_step():
    return drive_train()

