import sys, json, numpy as np, torch
from scipy.io import wavfile
from transformers import pipeline
W=sys.argv[1]; mid=sys.argv[2]; name=sys.argv[3]; step=float(sys.argv[4])
asr=pipeline('automatic-speech-recognition',model=mid,dtype=torch.float16,device='cuda')
sr,x=wavfile.read(f'{W}/ton/{name}.wav'); x=x.astype(np.float32)/32768
out=[]
L=int(step*sr)
kw={'task':'transcribe','num_beams':3}
if not mid.endswith('.en'): kw['language']='en'
for k in range(0,len(x),L):
    seg=x[k:k+L]
    if len(seg)<sr: break
    r=asr({'raw':seg,'sampling_rate':16000},return_timestamps=True,generate_kwargs=kw)
    for c in r['chunks']:
        a,b=c['timestamp']; a=(a or 0)+k/sr; b=(b if b is not None else step)+k/sr
        out.append((round(a,2),round(b,2),c['text']))
        print(f'{int(a//60)}:{a%60:05.2f}-{int(b//60)}:{b%60:05.2f} {c["text"]}',flush=True)
json.dump(out,open(f'{W}/ton/scan_{name}_{mid.split("/")[-1]}.json','w',encoding='utf-8'),indent=1,ensure_ascii=False)
