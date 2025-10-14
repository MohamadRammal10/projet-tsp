import tsplib95
import subprocess
import re
import sys
from pathlib import Path

# Used as reference to compare with the C program (make sure you compile c program first)
# Usage : python3 tests/test_tsplib95.py (in the root directory)

# ========= CONFIG =========
TESTS_DIR = Path("tests")
EXECUTABLE = Path("./tsp").resolve()
# ==========================

def run_c_prog(tsp_file):
    """Runs the compiled C executable and returns its stdout."""

    cmd = [str(EXECUTABLE), str(tsp_file)]
    try:
        result = subprocess.run(cmd, capture_output=True, text=True, check=True)
        return result.stdout
    except subprocess.CalledProcessError as e:
        print(f"stderr: {e.stderr}", file=sys.stderr)
        return None

def parse_c_output(output):
    """Extracts info from C debug output using regex."""
    info = {}

    if output is None:
        return info
    
    name_match = re.search(r"Name\s*:\s*(\S+)", output)
    dim_match = re.search(r"Dimension\s*:\s*(\d+)", output)
    edge_match = re.search(r"Edge weight type\s*:\s*(\S+)", output)

    if name_match:
        info["name"] = name_match.group(1)
    if dim_match:
        info["dimension"] = int(dim_match.group(1))
    if edge_match:
        info["edge_weight_type"] = edge_match.group(1)

    dist_matches = re.findall(r"\[DEBUG\] Read coords: (\d+) ->", output)
    info["coords_count"] = len(dist_matches)

    return info

def compare_with_tsplib95(problem, c_info):
    """Compares C prog with tsplib95 library."""
    results = []

    results.append(("Name", c_info.get("name"), problem.name))
    results.append(("Dimension", c_info.get("dimension"), problem.dimension))
    results.append(("Edge Weight Type", c_info.get("edge_weight_type"), problem.edge_weight_type))

    # Distance consistency check
    ref12 = problem.get_weight(1, 2)
    ref23 = problem.get_weight(2, 3)

    results.append(("Distance(1,2)", None, ref12))
    results.append(("Distance(2,3)", None, ref23))

    passed = True
    for label, c_val, py_val in results:
        if c_val is not None and str(c_val) != str(py_val):  # skip missing from C
            passed = False
    return passed, results

def test_all_files():
    """Run tests on all TSP files in the tests directory."""
    if not EXECUTABLE.exists():
        print(f"[ERROR] Executable {EXECUTABLE} NOT found.")
        sys.exit(1)
    
    tsp_files = sorted(TESTS_DIR.glob("*.tsp"))
    if not tsp_files:
        print(f"[ERROR] No .tsp files found in {TESTS_DIR}")
        sys.exit(2)

    print(f"Found {len(tsp_files)} TSP files\n")

    overall_pass = True
    for tsp_file in tsp_files:
        print(f"--- Testing {tsp_file.name} ---")
        c_output = run_c_prog(tsp_file)
        if c_output is None:
            overall_pass = False
            print(f"[ERROR] Failed to get output from C program for {tsp_file.name}\n")
            continue
        c_info = parse_c_output(c_output)
        problem = tsplib95.load(tsp_file)
        passed, results = compare_with_tsplib95(problem, c_info)

        status = "✅ PASS" if passed else "❌ FAIL"
        print(f"Result: {status}")

        for label, c_val, py_val in results:
            if c_val is None:
                print(f"{label:<25}: Python={py_val}")
            else:
                print(f"{label:<25}: C={c_val} | Python={py_val}")
        print("-" * 40)

        if not passed:
            overall_pass = False
    
    print("\n===== OVERALL TESTS RESULTS =====")
    if overall_pass:
        print("✅ ALL TESTS PASSED")
    else:
        print("❌ SOME TESTS FAILED")


if __name__ == "__main__":
    test_all_files()
