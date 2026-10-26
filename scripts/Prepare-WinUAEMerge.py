"""Prepare a three-way WinUAE source merge in scratch space for review."""

import argparse
import json
import pathlib
import subprocess


def blob(sha):
    return subprocess.check_output(["git", "cat-file", "blob", sha]).replace(b"\r\n", b"\n")


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--inventory", required=True)
    parser.add_argument("--scratch", required=True)
    args = parser.parse_args()
    inventory = json.loads(pathlib.Path(args.inventory).read_text(encoding="utf-8"))
    scratch = pathlib.Path(args.scratch)
    scratch.mkdir(parents=True, exist_ok=False)
    outcomes = []
    for item in inventory["mappings"]:
        if not item["status"].startswith("upstream_changed"):
            continue
        folder = scratch / item["local"]
        folder.parent.mkdir(parents=True, exist_ok=True)
        ours = folder.with_name(folder.name + ".ours")
        base = folder.with_name(folder.name + ".base")
        theirs = folder.with_name(folder.name + ".theirs")
        ours.write_bytes(blob(item["local_blob"]))
        base.write_bytes(blob(item["base_blob"]))
        theirs.write_bytes(blob(item["target_blob"]))
        merged = subprocess.run(["git", "merge-file", "-p", "--diff3", "-L", "PUAE", "-L", "WinUAE-base", "-L", "WinUAE-target", str(ours), str(base), str(theirs)], capture_output=True)
        if merged.returncode < 0 or merged.returncode > 127:
            raise RuntimeError(merged.stderr.decode(errors="replace"))
        folder.write_bytes(merged.stdout)
        outcomes.append({"local": item["local"], "upstream": item["upstream"], "conflicts": merged.stdout.count(b"<<<<<<< PUAE"), "exit_code": merged.returncode})
    (scratch / "outcomes.json").write_text(json.dumps(outcomes, indent=2) + "\n", encoding="utf-8")
    print(json.dumps({"prepared": len(outcomes), "conflicted_files": sum(bool(i["conflicts"]) for i in outcomes), "conflict_regions": sum(i["conflicts"] for i in outcomes), "conflicts": [i for i in outcomes if i["conflicts"]]}, indent=2))


if __name__ == "__main__":
    main()
