#!/usr/bin/env python3
"""
Codexion test harness.

Usage:
    python3 test_codexion.py [path_to_binary]

Defaults to ./codexion if no path is given. Run it from your project
directory after `make`, or pass the binary's path explicitly.
"""

import subprocess
import sys
import os
import re
import shutil

BINARY = sys.argv[1] if len(sys.argv) > 1 else "./codexion"

PASS = 0
FAIL = 0
RESULTS = []


def record(name, ok, detail=""):
    global PASS, FAIL
    status = "PASS" if ok else "FAIL"
    if ok:
        PASS += 1
    else:
        FAIL += 1
    RESULTS.append((name, status, detail))
    print(f"[{status}] {name}" + (f" -- {detail}" if detail and not ok else ""))


def run(args, timeout=10):
    """Run the binary with args, return (returncode, stdout, stderr, timed_out)."""
    try:
        p = subprocess.run([BINARY] + args, capture_output=True, text=True, timeout=timeout)
        return p.returncode, p.stdout, p.stderr, False
    except subprocess.TimeoutExpired as e:
        out = e.stdout.decode() if isinstance(e.stdout, bytes) else (e.stdout or "")
        err = e.stderr.decode() if isinstance(e.stderr, bytes) else (e.stderr or "")
        return None, out, err, True


def crashed(returncode):
    """A negative returncode on POSIX means killed by signal N (e.g. -11 = SIGSEGV)."""
    return returncode is not None and returncode < 0


# ---------- 1. Argument validation ----------

def test_invalid_args():
    bad_cases = [
        ([], "no arguments"),
        (["3", "100", "100", "100", "100", "5", "50"], "missing scheduler"),
        (["0", "100", "100", "100", "100", "5", "50", "fifo"], "zero coders"),
        (["-1", "100", "100", "100", "100", "5", "50", "fifo"], "negative coders"),
        (["abc", "100", "100", "100", "100", "5", "50", "fifo"], "non-numeric coders"),
        (["3", "100", "100", "100", "100", "5", "50", "roundrobin"], "invalid scheduler name"),
        (["3", "100", "100", "100", "100", "5", "50", "FIFO"], "wrong-case scheduler"),
        (["3", "-100", "100", "100", "100", "5", "50", "fifo"], "negative burnout"),
        (["3", "100", "100", "100", "100", "5", "50", "fifo", "extra"], "too many arguments"),
    ]
    for args, desc in bad_cases:
        rc, out, err, timed_out = run(args, timeout=3)
        if timed_out:
            record(f"invalid-args: {desc}", False, "hung / did not exit")
        elif crashed(rc):
            record(f"invalid-args: {desc}", False, f"crashed (signal {-rc})")
        elif rc == 0:
            record(f"invalid-args: {desc}", False, "accepted invalid input (exit 0)")
        else:
            record(f"invalid-args: {desc}", True)


# ---------- 2. Log parsing helpers ----------

LOG_RE = re.compile(r"^(\d+)\s+(\d+)\s+(.+)$")


def parse_log(output):
    """Return list of (time_ms, coder_id, status) tuples, in order."""
    events = []
    for line in output.splitlines():
        m = LOG_RE.match(line.strip())
        if m:
            events.append((int(m.group(1)), int(m.group(2)), m.group(3).strip()))
    return events


def dongles_for_coder(coder_id, nb_coders):
    """0-indexed dongle ids held by this 1-indexed coder, matching the
    project's left = i, right = (i+1) % n mapping, i = coder_id - 1."""
    i = coder_id - 1
    left = i % nb_coders
    right = (i + 1) % nb_coders
    return {left, right}


def find_overlap_violations(events, nb_coders):
    """
    Reconstruct each coder's [acquire_time, release_time) windows and check
    that no two coders sharing a dongle have overlapping windows.
    Acquire = timestamp of the 'has taken a dongle' pair.
    Release = the next logged status for that coder (when the compile
    phase ends and release_dongles runs).
    """
    per_coder_events = {}
    for t, cid, status in events:
        per_coder_events.setdefault(cid, []).append((t, status))

    intervals = []  # (coder_id, start, end)
    for cid, evs in per_coder_events.items():
        i = 0
        while i < len(evs):
            t, status = evs[i]
            if "taken a dongle" in status:
                start = t
                j = i + 1
                while j < len(evs) and "taken a dongle" in evs[j][1]:
                    j += 1
                end = evs[j][0] if j < len(evs) else start
                intervals.append((cid, start, end))
                i = j
            else:
                i += 1

    violations = []
    for a in range(len(intervals)):
        cid_a, sa, ea = intervals[a]
        dongles_a = dongles_for_coder(cid_a, nb_coders)
        for b in range(a + 1, len(intervals)):
            cid_b, sb, eb = intervals[b]
            if cid_a == cid_b:
                continue
            dongles_b = dongles_for_coder(cid_b, nb_coders)
            if not (dongles_a & dongles_b):
                continue
            if sa < eb and sb < ea:
                violations.append((cid_a, sa, ea, cid_b, sb, eb, dongles_a & dongles_b))
    return violations


# ---------- 3. Functional / correctness tests ----------

def test_single_coder():
    args = ["1", "5000", "100", "100", "100", "3", "50", "fifo"]
    rc, out, err, timed_out = run(args, timeout=8)
    if timed_out:
        record("single coder completes", False, "timed out / hung")
        return
    if crashed(rc):
        record("single coder completes", False, f"crashed (signal {-rc})")
        return
    events = parse_log(out)
    compiles = sum(1 for _, cid, s in events if cid == 1 and s == "is compiling")
    record("single coder completes", rc == 0 and compiles >= 3,
           f"exit={rc}, compiles={compiles}, stderr={err.strip()[:200]}")


def test_no_deadlock_and_no_overlap(nb_coders, scheduler, label, req=8, burnout=10000):
    args = [str(nb_coders), str(burnout), "100", "100", "100", str(req), "50", scheduler]
    rc, out, err, timed_out = run(args, timeout=15)
    if timed_out:
        record(f"{label}: terminates", False, "possible deadlock (timed out)")
        return
    if crashed(rc):
        record(f"{label}: terminates", False, f"crashed (signal {-rc})")
        return
    record(f"{label}: terminates", True, f"exit={rc}")

    events = parse_log(out)
    violations = find_overlap_violations(events, nb_coders)
    record(f"{label}: no shared-dongle overlap", len(violations) == 0,
           f"{len(violations)} violation(s), e.g. {violations[0] if violations else ''}")

    counts = {}
    for _, cid, s in events:
        if s == "is compiling":
            counts[cid] = counts.get(cid, 0) + 1
    all_met = all(counts.get(cid, 0) >= req for cid in range(1, nb_coders + 1))
    record(f"{label}: every coder reached compile target", all_met, f"counts={counts}")


def test_burnout_detected():
    # burnout window far too small for anyone to realistically compile in time
    args = ["4", "150", "200", "200", "200", "10", "100", "fifo"]
    rc, out, err, timed_out = run(args, timeout=10)
    if timed_out:
        record("burnout: terminates", False, "did not stop after burnout should occur")
        return
    events = parse_log(out)
    burned = [e for e in events if "burned out" in e[2]]
    record("burnout: message logged", len(burned) >= 1,
           f"found {len(burned)} burnout message(s)")
    if not burned:
        return
    burn_time = burned[0][0]
    later_activity = [e for e in events if e[0] > burn_time + 5
                       and e[2] in ("is compiling", "is debugging", "is refactoring")]
    record("burnout: simulation halts after burnout", len(later_activity) == 0,
           f"{len(later_activity)} event(s) logged after burnout")


def test_two_coders_share_correctly():
    test_no_deadlock_and_no_overlap(2, "fifo", "2 coders (fifo)", req=5, burnout=8000)


def test_single_coder_dongle_is_shared_with_self():
    # With 1 coder both hands are the same dongle; confirms no deadlock
    # and correct acquire/release pairing on the self-shared case.
    test_no_deadlock_and_no_overlap(1, "fifo", "1 coder (self-shared dongle)", req=3, burnout=8000)


def test_valgrind():
    if not shutil.which("valgrind"):
        record("valgrind: memory check", True, "skipped (valgrind not installed)")
        return
    args = ["valgrind", "--leak-check=full", "--error-exitcode=97", BINARY,
            "4", "8000", "80", "80", "80", "5", "40", "fifo"]
    try:
        p = subprocess.run(args, capture_output=True, text=True, timeout=30)
    except subprocess.TimeoutExpired:
        record("valgrind: memory check", False, "valgrind run timed out")
        return
    no_definite_leak = "definitely lost: 0 bytes" in p.stderr or "definitely lost" not in p.stderr
    no_errors = p.returncode != 97
    ok = no_definite_leak and no_errors
    record("valgrind: no definite leaks / errors", ok, "" if ok else p.stderr[-800:])


# ---------- main ----------

def main():
    if not os.path.exists(BINARY):
        print(f"Binary not found at '{BINARY}'. Build it first (make), "
              f"or pass the path: python3 test_codexion.py ./path/to/codexion")
        sys.exit(1)

    print("== Argument validation ==")
    test_invalid_args()

    print("\n== Basic correctness ==")
    test_single_coder()
    test_single_coder_dongle_is_shared_with_self()
    test_two_coders_share_correctly()

    print("\n== Contention / scheduler tests ==")
    test_no_deadlock_and_no_overlap(5, "fifo", "5 coders (fifo, tight timing)", req=8, burnout=10000)
    test_no_deadlock_and_no_overlap(5, "edf", "5 coders (edf, tight timing)", req=8, burnout=10000)
    test_no_deadlock_and_no_overlap(9, "fifo", "9 coders (fifo, larger ring)", req=5, burnout=15000)

    print("\n== Stop conditions ==")
    test_burnout_detected()

    print("\n== Memory ==")
    test_valgrind()

    print(f"\n{'=' * 40}\n{PASS} passed, {FAIL} failed\n{'=' * 40}")
    sys.exit(1 if FAIL else 0)


if __name__ == "__main__":
    main()
