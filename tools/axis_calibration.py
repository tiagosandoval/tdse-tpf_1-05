#!/usr/bin/env python3
"""
Determines which physical MPU6050 axis is the servo's true shaft axis, from
a TeraTerm log captured while running the firmware in TEST_AXIS_CAL mode
(see app/inc/app.h). The firmware drives the servo through a slow, known
triangle-wave sweep and logs the commanded angle alongside all six raw IMU
channels, one line per sample:

    t=<tick_ms>,c=<cmd_deg>,ax=<g>,ay=<g>,az=<g>,gx=<dps>,gy=<dps>,gz=<dps>

Physical assumption (see the plan): the servo shaft is mounted horizontal,
so the arm's plane of rotation is vertical and contains the gravity vector.
That means gravity's component along the true shaft axis is ~constant for
every servo angle -- the accelerometer channel aligned with the shaft should
have near-zero variance across the whole sweep, while the other two swing
with the arm. This gives two independent ways to find the shaft axis:

  1. accel variance   -- lowest-variance accel channel = shaft axis
  2. gyro correlation -- gyro channel best matching cmd_deg's derivative
                          should be the *same* physical axis index

The two accel channels *not* picked as the shaft axis are the in-plane
pair; atan2(pair) fit against cmd_deg (in both role assignments) picks the
correct roles and sign for imu_filter_update().

Usage:
    python tools/axis_calibration.py <teraterm_logfile>
"""

import re
import sys

LINE_RE = re.compile(
    r"t=(?P<t>\d+),c=(?P<c>-?\d+\.?\d*),"
    r"ax=(?P<ax>-?\d+\.?\d*),ay=(?P<ay>-?\d+\.?\d*),az=(?P<az>-?\d+\.?\d*),"
    r"gx=(?P<gx>-?\d+\.?\d*),gy=(?P<gy>-?\d+\.?\d*),gz=(?P<gz>-?\d+\.?\d*)"
)

AXES = ("x", "y", "z")


def parse(path):
    samples = []
    with open(path, "r", errors="ignore") as f:
        for line in f:
            m = LINE_RE.search(line)
            if not m:
                continue
            d = m.groupdict()
            samples.append({k: float(v) for k, v in d.items()})
    return samples


def mean(xs):
    return sum(xs) / len(xs)


def variance(xs):
    mu = mean(xs)
    return sum((x - mu) ** 2 for x in xs) / len(xs)


def linreg(xs, ys):
    """Least-squares slope/intercept/correlation of ys = slope*xs + intercept."""
    n = len(xs)
    mx, my = mean(xs), mean(ys)
    sxy = sum((x - mx) * (y - my) for x, y in zip(xs, ys))
    sxx = sum((x - mx) ** 2 for x in xs)
    syy = sum((y - my) ** 2 for y in ys)
    if sxx == 0 or syy == 0:
        return 0.0, my, 0.0
    slope = sxy / sxx
    intercept = my - slope * mx
    corr = sxy / (sxx ** 0.5 * syy ** 0.5)
    return slope, intercept, corr


def cmd_rate(samples):
    """Numerically differentiate cmd_deg w.r.t. real time (deg/s), one value
    per interior sample (endpoints dropped, matching len(samples)-2)."""
    rates = []
    for i in range(1, len(samples) - 1):
        dt = (samples[i + 1]["t"] - samples[i - 1]["t"]) / 1000.0  # s
        if dt <= 0:
            rates.append(0.0)
            continue
        rates.append((samples[i + 1]["c"] - samples[i - 1]["c"]) / dt)
    return rates


def atan2_deg(y, x):
    import math
    return math.degrees(math.atan2(y, x))


def main():
    if len(sys.argv) != 2:
        print(f"usage: {sys.argv[0]} <teraterm_logfile>")
        sys.exit(1)

    samples = parse(sys.argv[1])
    if len(samples) < 20:
        print(f"only found {len(samples)} matching samples -- did the sweep run "
              f"long enough, and does the log contain 't=...,c=...,ax=...' lines?")
        sys.exit(1)

    print(f"parsed {len(samples)} samples\n")

    # --- 1. accel variance test: lowest-variance accel channel = shaft axis ---
    accel_var = {}
    for axis in AXES:
        vals = [s[f"a{axis}"] for s in samples]
        accel_var[axis] = variance(vals)

    print("accel variance per axis (lowest = shaft axis candidate):")
    for axis, v in sorted(accel_var.items(), key=lambda kv: kv[1]):
        print(f"  a{axis}: variance={v:.5f}")
    shaft_axis_accel = min(accel_var, key=accel_var.get)
    print(f"  -> accel test picks shaft axis: {shaft_axis_accel}\n")

    # --- 2. gyro correlation test: best match to cmd_deg's derivative ---
    rates = cmd_rate(samples)
    interior = samples[1:-1]

    gyro_fit = {}
    for axis in AXES:
        vals = [s[f"g{axis}"] for s in interior]
        slope, intercept, corr = linreg(rates, vals)
        gyro_fit[axis] = (slope, intercept, corr)

    print("gyro channel vs. cmd_deg rate (slope should be ~+-1, corr should be ~+-1):")
    for axis, (slope, intercept, corr) in sorted(
            gyro_fit.items(), key=lambda kv: -abs(kv[1][2])):
        print(f"  g{axis}: slope={slope:+.3f} intercept={intercept:+.3f} corr={corr:+.3f}")
    shaft_axis_gyro = max(gyro_fit, key=lambda a: abs(gyro_fit[a][2]))
    gyro_sign = 1 if gyro_fit[shaft_axis_gyro][0] >= 0 else -1
    print(f"  -> gyro test picks shaft axis: {shaft_axis_gyro} "
          f"(sign {'+' if gyro_sign > 0 else '-'})\n")

    if shaft_axis_accel != shaft_axis_gyro:
        print(f"*** MISMATCH: accel test says '{shaft_axis_accel}', "
              f"gyro test says '{shaft_axis_gyro}'. Trust the gyro correlation "
              f"(direct match to a commanded rate) but re-check the mount / "
              f"re-run the sweep before deciding.\n")
        shaft_axis = shaft_axis_gyro
    else:
        print(f"both tests agree: shaft axis = {shaft_axis_accel}\n")
        shaft_axis = shaft_axis_accel

    # --- 3. tilt-pair fit: the other two accel axes, both role assignments ---
    other_axes = [a for a in AXES if a != shaft_axis]
    p, q = other_axes

    cmds = [s["c"] for s in samples]
    best = None
    for num_axis, den_axis in ((p, q), (q, p)):
        tilts = [atan2_deg(s[f"a{num_axis}"], s[f"a{den_axis}"]) for s in samples]
        slope, intercept, corr = linreg(cmds, tilts)
        print(f"tilt = atan2(a{num_axis}, a{den_axis}) vs cmd_deg: "
              f"slope={slope:+.3f} intercept={intercept:+.3f} corr={corr:+.3f}")
        if best is None or abs(corr) > abs(best[3]):
            best = (num_axis, den_axis, slope, corr)

    num_axis, den_axis, slope, corr = best
    sign = "+" if slope >= 0 else "-"
    print(f"\n  -> best tilt-pair fit: atan2(a{num_axis}, a{den_axis}), "
          f"sign={sign}, corr={corr:+.3f}\n")

    print("=" * 60)
    print("RECOMMENDATION for imu_filter_update()'s call site in task_imu.c:")
    print(f"  rate term  : g{shaft_axis}_dps  (sign {'+' if gyro_sign > 0 else '-'})")
    print(f"  tilt term  : atan2(a{num_axis}_g, a{den_axis}_g)  (sign {sign})")
    print("=" * 60)


if __name__ == "__main__":
    main()
