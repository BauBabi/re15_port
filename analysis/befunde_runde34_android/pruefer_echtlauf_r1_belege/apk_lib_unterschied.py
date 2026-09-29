import zipfile, sys, re
ref = zipfile.ZipFile(sys.argv[1]); neu = zipfile.ZipFile(sys.argv[2])
for n in ["lib/arm64-v8a/libmain.so", "lib/x86_64/libmain.so", "lib/arm64-v8a/libSDL2.so", "lib/x86_64/libSDL2.so"]:
    a = ref.read(n); b = neu.read(n)
    diff = sum(1 for x, y in zip(a, b) if x != y) if len(a) == len(b) else None
    pa = sorted(set(m.group(0).decode("latin1") for m in re.finditer(rb"[A-Za-z]:[/\][ -~]{0,120}?(?:re15_port|reAi_v2)[ -~]{0,40}", a)))[:3]
    pb = sorted(set(m.group(0).decode("latin1") for m in re.finditer(rb"[A-Za-z]:[/\][ -~]{0,120}?(?:re15_port|reAi_v2)[ -~]{0,40}", b)))[:3]
    na = a.count(b"worktrees"); nb = b.count(b"worktrees")
    print(n, "Groesse", len(a), len(b), "abweichende Bytes (gleiche Laenge):", diff, "| 'worktrees' ref/neu:", na, nb)
    print("   ref-Pfade:", pa); print("   neu-Pfade:", pb)
