from pathlib import Path
import sys

sys.path.insert(0, str(Path(__file__).resolve().parents[1]))

from automatic_nail_machine import feed_and_straightening


def gen_step():
    return feed_and_straightening()

