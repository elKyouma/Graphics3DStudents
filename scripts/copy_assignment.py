#!/usr/bin/env python3

import argparse
import shutil
from pathlib import Path
import re
import sys

parser = argparse.ArgumentParser(description="Copy Assignment")
parser.add_argument("-f", "--force", action="store_true", help="overwrite the destination if it exists")
parser.add_argument("source", metavar="source", action="store")
parser.add_argument("dest", metavar="dest", action="store")

args = parser.parse_args()

ROOT_PATH = Path(__file__).resolve().parent.parent
ASSIGNMENTS_PATH = ROOT_PATH / "src" / "Assignments"

if not ASSIGNMENTS_PATH.is_dir():
    print(f"Cannot find assignments directory {ASSIGNMENTS_PATH}")
    sys.exit(1)

for name in (args.source, args.dest):
    if Path(name).name != name or name in (".", ".."):
        print(f"{name} should be a name of a directory in {ASSIGNMENTS_PATH}, not a path")
        sys.exit(1)

# The destination name becomes the name of the CMake target, so it may contain only letters, digits and underscores.
if not re.fullmatch(r"\w+", args.dest, re.ASCII):
    print(f"{args.dest} should contain only letters, digits and underscores, e.g. 01_House")
    sys.exit(1)

# Names of the other targets of the project; an assignment with the same name would make the configuration fail.
RESERVED_NAMES = {"application", "Engine", "OGL", "objreader", "mikktspace", "glad", "test", "glfw", "glm", "spdlog",
                  "cxxopts", "imgui", "Catch2"}
if args.dest in RESERVED_NAMES:
    print(f"{args.dest} is the name of another target of the project, choose a different name")
    sys.exit(1)

if args.source == args.dest:
    print("Source and destination must be different")
    sys.exit(1)

source_path = ASSIGNMENTS_PATH / args.source

if not source_path.is_dir():
    print(f"Source {args.source} is not a subdirectory of {ASSIGNMENTS_PATH}")
    sys.exit(1)

dest_path = ASSIGNMENTS_PATH / args.dest

if dest_path.exists() and not args.force:
    print(f"Destination {args.dest} exists. Use --force flag to overwrite")
    sys.exit(1)

# The copy is made in a temporary directory and only then replaces the destination, so if anything fails, an existing
# destination is left as it was.
tmp_path = ASSIGNMENTS_PATH / f".{args.dest}.tmp"
old_path = ASSIGNMENTS_PATH / f".{args.dest}.old"
try:
    for path in (tmp_path, old_path):
        if path.exists():
            shutil.rmtree(path)
    shutil.copytree(source_path, tmp_path)

    cmake_lists_path = tmp_path / "CMakeLists.txt"
    cmake_lists_txt = cmake_lists_path.read_text()

    # The project, and so the executable target, has the same name as the assignment directory, e.g. 01_House.
    # Directory names are unique, so are the target names.
    project_re = re.compile(r"project\(\s*(\w+)\s*\)", re.I)
    cmake_lists_txt = project_re.sub("project(" + args.dest + ")", cmake_lists_txt)
    cmake_lists_path.write_text(cmake_lists_txt)

    if dest_path.exists():
        dest_path.rename(old_path)
    tmp_path.rename(dest_path)
    if old_path.exists():
        shutil.rmtree(old_path)
except Exception as ex:
    print(ex)
    if tmp_path.exists():
        shutil.rmtree(tmp_path, ignore_errors=True)
    if old_path.exists() and not dest_path.exists():
        old_path.rename(dest_path)
    sys.exit(1)

# Only assignments listed in the top CMakeLists.txt are built.
top_cmake_lists_txt = (ROOT_PATH / "CMakeLists.txt").read_text()
assignments_match = re.search(r"set\(\s*ASSIGNMENTS\s+([^)]*)\)", top_cmake_lists_txt)
if assignments_match and args.dest not in assignments_match.group(1).split():
    print(f"Warning: {args.dest} is not on the ASSIGNMENTS list in {ROOT_PATH / 'CMakeLists.txt'} "
          f"and will not be built")
