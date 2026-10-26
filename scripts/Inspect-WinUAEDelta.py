"""Inventory a WinUAE vendor delta against this repository without editing sources."""

import argparse
import collections
import json
import pathlib
import re
import subprocess


def git(*args):
    return subprocess.check_output(["git", *args])


def tree(ref):
    result = {}
    for entry in git("ls-tree", "-r", "-z", ref).split(b"\0"):
        if not entry:
            continue
        fields, path = entry.split(b"\t", 1)
        mode, kind, sha = fields.decode().split()
        if kind == "blob":
            result[path.decode()] = sha
    return result


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--base", default="51b7be351f359dc97f967f051da390283bc4b409")
    parser.add_argument("--target", default="winuae/master")
    parser.add_argument("--output", required=True)
    args = parser.parse_args()
    base = tree(args.base)
    target = tree(args.target)
    local = tree("HEAD")
    mappings = []
    for path, sha in local.items():
        if not path.startswith("sources/src/"):
            continue
        relative = path.removeprefix("sources/src/")
        variants = [relative]
        if relative.endswith(".c"):
            variants.append(relative[:-2] + ".cpp")
        if relative.startswith("include/"):
            variants.append(relative.removeprefix("include/"))
        match = next((candidate for candidate in variants if candidate in base), None)
        if match is None:
            state = "local_only"
        elif match not in target:
            state = "upstream_deleted"
        elif base[match] == target[match]:
            state = "upstream_unchanged"
        elif sha == base[match]:
            state = "upstream_changed_local_equal_base"
        else:
            state = "upstream_changed_local_modified"
        mappings.append({"local": path, "upstream": match, "status": state,
                         "local_blob": sha, "base_blob": base.get(match), "target_blob": target.get(match)})
    counts = dict(collections.Counter(item["status"] for item in mappings))
    mapped = {item["upstream"] for item in mappings if item["upstream"]}
    report = {"base": git("rev-parse", args.base).decode().strip(),
              "target": git("rev-parse", args.target).decode().strip(),
              "local_head": git("rev-parse", "HEAD").decode().strip(),
              "counts": counts, "mappings": mappings,
              "new_upstream_files": sorted(set(target) - set(base)),
              "changed_unmapped_upstream_files": sorted(path for path in base if path in target and base[path] != target[path] and path not in mapped)}
    output = pathlib.Path(args.output)
    output.parent.mkdir(parents=True, exist_ok=True)
    output.write_text(json.dumps(report, indent=2) + "\n", encoding="utf-8")
    print(json.dumps({"base": report["base"], "target": report["target"], "counts": counts,
                      "new_upstream_files": len(report["new_upstream_files"]),
                      "changed_unmapped_upstream_files": len(report["changed_unmapped_upstream_files"])}, indent=2))


if __name__ == "__main__":
    main()
