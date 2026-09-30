# Ersetzt genau einmal old->new in einer Datei und behaelt deren Zeilenenden (CRLF/LF).
# Aufruf: python sub.py <datei> <old-datei> <new-datei>
import sys
p, fo, fn = sys.argv[1:4]
b=open(p,'rb').read()
crlf = b'\r\n' in b
s=b.decode('utf-8').replace('\r\n','\n')
old=open(fo,encoding='utf-8').read().replace('\r\n','\n')
new=open(fn,encoding='utf-8').read().replace('\r\n','\n')
n=s.count(old)
if n!=1: raise SystemExit('FEHLER: %d Treffer in %s'%(n,p))
s=s.replace(old,new,1)
if crlf: s=s.replace('\n','\r\n')
open(p,'wb').write(s.encode('utf-8'))
print('ok',p)
