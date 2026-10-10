import argparse
import filecmp
from pathlib import Path
import shutil

parser = argparse.ArgumentParser(description="Import existing game font assets into a game user data directory.")
parser.add_argument("source", type=Path)
parser.add_argument("user_data", type=Path)
args = parser.parse_args()
required = ["arial.ttf", "arialbd.ttf", "ariali.ttf", "arialbi.ttf", "times.ttf", "timesbd.ttf", "timesi.ttf", "timesbi.ttf", "cour.ttf", "courbd.ttf", "couri.ttf", "courbi.ttf", "vgafix.fon", "8514fix.fon", "coure.fon", "courf.fon"]
files = {p.name.casefold(): p for p in args.source.iterdir() if p.is_file()}
missing = [name for name in required if name not in files]
if missing:
    parser.error("Missing font assets: " + ", ".join(missing))
destination = args.user_data / "Fonts"
destination.mkdir(parents=True, exist_ok=True)
for name in required + (["arialuni.ttf"] if "arialuni.ttf" in files else []):
    target = destination / name
    if files[name].resolve() != target.resolve() and not (target.is_file() and filecmp.cmp(files[name], target, shallow=False)):
        temporary = target.with_suffix(target.suffix + ".importing")
        shutil.copyfile(files[name], temporary)
        temporary.replace(target)
    print(target)
