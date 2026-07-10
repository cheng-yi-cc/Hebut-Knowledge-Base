from pathlib import Path
import sys

sys.path.insert(0, str(Path(__file__).resolve().parents[1]))

from automatic_nail_machine import heading_station


def gen_step():
    return heading_station()

