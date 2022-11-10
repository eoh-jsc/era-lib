Import("env")
from os.path import dirname, join
cwd = env.subst('$PROJECT_DIR')

def endings(f, lines):
    for line in lines:
        if line.endswith("\n") == False:
            f.write("\n")

def find(lines, str):
    for line in lines:
        if str in line:
            return True
    return False

def replace(f, lines, old, new):
    found = False
    for line in lines:
        if old in line:
            found = True
    if found == False:
        f.write(new)

with open(join(cwd, './platformio.ini'), 'r+') as f:
    lines = f.readlines()
    endings(f, lines)
    if find(lines, "espressif32") == True:
        replace(f, lines, "monitor_speed", "monitor_speed = 115200\n");
        replace(f, lines, "upload_speed", "upload_speed = 921600\n");
        replace(f, lines, "upload_port", "; upload_port = *\n");
        replace(f, lines, "board_build.partitions", "board_build.partitions = .pio/libdeps/$PIOENV/ERa/era_partition.csv\n");
    elif find(lines, "espressif8266") == True:
        replace(f, lines, "monitor_speed", "monitor_speed = 115200\n");
        replace(f, lines, "upload_speed", "upload_speed = 921600\n");
        replace(f, lines, "upload_port", "; upload_port = *\n");
    elif find(lines, "ststm32") == True:
        replace(f, lines, "upload_protocol", "upload_protocol = stlink\n");
