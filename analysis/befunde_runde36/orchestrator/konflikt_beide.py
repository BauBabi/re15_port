import io, sys
for p in sys.argv[1:]:
    s = io.open(p, encoding='utf-8', newline='').read()
    nl = '\r\n' if '\r\n' in s else '\n'
    lines = s.split(nl); out = []; state = 0
    for l in lines:
        if l.startswith('<<<<<<< '): state = 1; continue
        if state and l.startswith('=======') and l.strip() == '=======': state = 2; continue
        if state and l.startswith('>>>>>>> '): state = 0; continue
        out.append(l)
    io.open(p, 'w', encoding='utf-8', newline='').write(nl.join(out))
    print(p, 'ok')
