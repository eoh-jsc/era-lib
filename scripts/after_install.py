Import("env")

import glob
import os
import re
import shutil

project_dir = env.subst("$PROJECT_DIR")
pio_env = env.subst("$PIOENV")
platformio_ini_path = os.path.join(project_dir, "platformio.ini")

src_dir = os.path.join(project_dir, ".pio", "libdeps", pio_env, "ERa")
pattern = os.path.join(src_dir, "era_partition*.csv")
files = glob.glob(pattern)

for src_file in files:
    dst_file = os.path.join(project_dir, os.path.basename(src_file))
    if not os.path.isfile(dst_file):
        shutil.copy(src_file, dst_file)
        print(f"[INFO] Copied {src_file} -> {dst_file}")
    else:
        print(f"[INFO] Skipped (already exists): {dst_file}")

platform = env.BoardConfig().get("platform")

if "espressif32" in platform:
    print(f"[INFO] Platform is ESP32 - checking platformio.ini section [env:{pio_env}]")

    with open(platformio_ini_path, "r") as f:
        lines = f.readlines()

    section_start = None
    section_end = None
    for i, line in enumerate(lines):
        if re.match(rf"\[env:{re.escape(pio_env)}\]", line.strip()):
            section_start = i
            for j in range(i+1, len(lines)):
                if lines[j].strip().startswith("[env:"):
                    section_end = j
                    break
            else:
                section_end = len(lines)
            break

    if section_start is not None:
        section_lines = lines[section_start:section_end]
        has_partition = any("board_build.partitions" in line.strip() for line in section_lines)

        if not has_partition:
            board_name = None
            for line in section_lines:
                line_strip = line.strip()
                if line_strip.startswith("board ="):
                    board_name = line_strip.split("=", 1)[1].strip()
                    break

            inserted = False
            if board_name:
                for i in range(section_start + 1, section_end):
                    if lines[i].strip() == f"board = {board_name}":
                        insertion_index = i + 1
                        if not lines[i].endswith("\n"):
                            lines[i].append("\n")
                        lines.insert(insertion_index, "board_build.partitions = era_partition.csv\n")
                        inserted = True
                        break

            if inserted:
                with open(platformio_ini_path, "w") as f:
                    f.writelines(lines)
                print(f"[INFO] Added board_build.partitions = era_partition.csv after board = {board_name}")
            else:
                print(f"[WARN] Could not find 'board = ...' in [env:{pio_env}] — skipping insert.")
        else:
            print(f"[INFO] [env:{pio_env}] already has board_build.partitions — skipping.")
    else:
        print(f"[WARN] Could not find [env:{pio_env}] section in platformio.ini")
