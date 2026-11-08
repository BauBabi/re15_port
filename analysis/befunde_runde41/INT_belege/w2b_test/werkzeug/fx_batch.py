# fx_batch.py <basis> <parallel> <ausgabe> -- zeilen: name teil elza pos vorne exit_at [ohne] [fd=...]
import subprocess, sys, os
from concurrent.futures import ThreadPoolExecutor
basis, par, aus = sys.argv[1], int(sys.argv[2]), sys.argv[3]
liste = [l.split() for l in sys.stdin.read().splitlines() if l.strip() and not l.startswith("#")]
tool = os.path.join(os.path.dirname(os.path.abspath(__file__)), "fx_lauf.py")
def lauf(a):
    r = subprocess.run([sys.executable, "-I", tool, basis] + a, capture_output=True, text=True)
    return (r.stdout + r.stderr).strip()
with ThreadPoolExecutor(par) as ex:
    erg = list(ex.map(lauf, liste))
with open(aus, "a") as f:
    for e in erg:
        f.write(e + "\n")
print("\n".join(erg))
