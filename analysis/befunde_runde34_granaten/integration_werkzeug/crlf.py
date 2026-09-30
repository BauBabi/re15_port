# Setzt CRLF-Zeilenenden fuer die genannten Dateien (Arbeitskopie war CRLF, core.autocrlf=true).
import sys
for p in sys.argv[1:]:
    b=open(p,'rb').read()
    b=b.replace(b'\r\n',b'\n').replace(b'\n',b'\r\n')
    open(p,'wb').write(b)
