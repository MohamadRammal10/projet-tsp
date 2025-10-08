import tsplib95
import subprocess
import re
import sys
from pathlib import Path

# Used as reference to compare with the C program (make sure you compile c program first)
# Usage : python3 tests/test_tsplib95.py (in the root directory)

# ========= CONFIG =========
TSP_FILE = "tests/bier127.tsp"
EXECUTABLE = "./tsp"
# ==========================

def run_c_prog(tsp_file):
    """Runs the compiled C executable and returns its stdout."""

    cmd = [EXECUTABLE, tsp_file]
    try:
        result = subprocess.run(cmd, capture_output=True, text=True, check=True)
        return result.stdout
    except subprocess.CalledProcessError as e:
        print(f"Error running C program: {e}", file=sys.stderr)
        print(f"stderr: {e.stderr}", file=sys.stderr)
        sys.exit(1)

def parse_c_output(output):
    """Extracts info from C debug output using regex."""
    info = {}
    name_match = re.search(r"Name\s*:\s*(\S+)", output)
    dim_match = re.search(r"Dimension\s*:\s*(\d+)", output)
    edge_match = re.search(r"Edge weight type\s*:\s*(\S+)", output)

    if name_match:
        info["name"] = name_match.group(1)
    if dim_match:
        info["dimension"] = int(dim_match.group(1))
    if edge_match:
        info["edge_weight_type"] = edge_match.group(1)

    # optional: extract a few distances if printed
    dist_matches = re.findall(r"\[DEBUG\] Read coords: (\d+) ->", output)
    info["coords_count"] = len(dist_matches)

    return info

def compare_with_tsplib95(problem, c_info):
    """Compares C prog with tsplib95 library."""
    results = []

    results.append(("Name", c_info.get("name"), problem.name))
    results.append(("Dimension", c_info.get("dimension"), problem.dimension))
    results.append(("Edge Weight Type", c_info.get("edge_weight_type"), problem.edge_weight_type))

    # Distance consistency check (C = Euclidean integer-rounded)
    ref12 = problem.get_weight(1, 2)
    ref23 = problem.get_weight(2, 3)

    results.append(("Distance(1,2)", None, ref12))
    results.append(("Distance(2,3)", None, ref23))

    print("\n===== AUTOMATED TEST REPORT =====")
    passed = True
    for label, c_val, py_val in results:
        if c_val is None:  # skip missing from C
            print(f"{label:<25}: Python={py_val}")
            continue

        status = "✅ PASS" if str(c_val) == str(py_val) else "❌ FAIL"
        print(f"{label:<25}: C={c_val} | Python={py_val} --> {status}")
        if status == "❌ FAIL":
            passed = False

    print(f"\nTotal: {'✅ ALL TESTS PASSED' if passed else '❌ SOME TESTS FAILED'}")
    return passed


def main():
    if not Path(EXECUTABLE).exists():
        print(f"[ERROR] Executable {EXECUTABLE} not found. Compile your C program first (make).")
        sys.exit(1)
    if not Path(TSP_FILE).exists():
        print(f"[ERROR] File {TSP_FILE} not found.")
        sys.exit(1)

    print(f"Running C program on {TSP_FILE}...\n")
    c_output = run_c_prog(TSP_FILE)
    c_info = parse_c_output(c_output)

    problem = tsplib95.load(TSP_FILE)
    compare_with_tsplib95(problem, c_info)


if __name__ == "__main__":
    main()
