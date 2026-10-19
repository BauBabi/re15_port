import sys; sys.path.insert(0,'.')
from modell import *
CH={'P':'.','B':'1','W':'#','X':'x','D':'D','S':'S','C':'c','O':'o','BB':'b','T':'t','CW':'w','CC':'k'}
def zeig(seite,x0,x1,y0,y1,welche=('ist','soll')):
    S=Seite(seite)
    for wer in welche:
        a=getattr(S,wer)
        print(wer, 'x', x0, '..', x1)
        print('     '+''.join(str((x//10)%10) for x in range(x0,x1+1)))
        print('     '+''.join(str(x%10) for x in range(x0,x1+1)))
        for y in range(y0,y1+1):
            print('%4d '%y+''.join(CH[NAME[a[y,x]]] for x in range(x0,x1+1)))
if __name__=='__main__':
    s,x0,x1,y0,y1=map(int,sys.argv[1:6]); zeig(s,x0,x1,y0,y1)
