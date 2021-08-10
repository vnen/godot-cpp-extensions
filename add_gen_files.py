#!/usr/bin/env python

import glob
from pathlib import Path

files = [f for f in glob.glob("gen/**/*.cpp", recursive=True)]

for f in files:
    print(f)

Path("meson.build").touch()
