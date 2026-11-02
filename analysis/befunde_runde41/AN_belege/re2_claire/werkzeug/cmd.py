"""cmd.py CMDFILE LOGFILE "befehl1; befehl2; ..." [timeout] - Befehle an re2c_drv.py anhaengen und auf die eigene Marke warten."""
import sys, time, os, random
cmdf, logf, cmds = sys.argv[1], sys.argv[2], sys.argv[3]
to = float(sys.argv[4]) if len(sys.argv) > 4 else 300
mid = "m%06d" % random.randint(0, 999999)
n0 = len(open(logf).read().splitlines()) if os.path.exists(logf) else 0
with open(cmdf, "a") as f:
    for c in cmds.split(";"):
        if c.strip():
            f.write(c.strip() + "\n")
    f.write("mark %s\n" % mid)
t0 = time.time()
while time.time() - t0 < to:
    s = open(logf).read() if os.path.exists(logf) else ""
    if ("done mark " + mid) in s or "duck exited" in s:
        break
    time.sleep(0.3)
ls = open(logf).read().splitlines()[n0:]
print("\n".join(l for l in ls if " cmd " not in l and "done mark" not in l)[-3000:])
