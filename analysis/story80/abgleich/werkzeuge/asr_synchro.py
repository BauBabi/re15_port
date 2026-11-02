# ST80-A: Spracherkennung der Nutzer-Sprachdateien unter synchro/ (nur lesen).
# Aufruf: HF_HOME=<Modell-Cache> python asr_synchro.py <repo>/synchro <ausgabe.json> STAGE3/room30E1/main02.wav ...
# Modelle: openai/whisper-large-v3-turbo (language=en) und openai/whisper-medium.en, Beam 5, CUDA fp16.
# Ergebnis dieser Spur: ../spracherkennung.json (62 Dateien).
import sys, json, wave, numpy as np, torch
from scipy.signal import resample_poly
from math import gcd
from transformers import pipeline
root=sys.argv[1]; out=sys.argv[2]; files=sys.argv[3:]
def load(p):
    w=wave.open(p); n=w.getnframes(); sr=w.getframerate(); ch=w.getnchannels(); sw=w.getsampwidth()
    b=w.readframes(n)
    x=np.frombuffer(b,dtype={1:np.uint8,2:np.int16,4:np.int32}[sw]).astype(np.float32)
    if sw==1: x=(x-128)/128
    elif sw==2: x/=32768
    else: x/=2**31
    if ch>1: x=x.reshape(-1,ch).mean(1)
    g=gcd(sr,16000); x=resample_poly(x,16000//g,sr//g).astype(np.float32)
    return x, sr, n/sr
res={}
for mid in ['openai/whisper-large-v3-turbo','openai/whisper-medium.en']:
    asr=pipeline('automatic-speech-recognition',model=mid,torch_dtype=torch.float16,device='cuda')
    kw={'num_beams':5} if mid.endswith('.en') else {'language':'en','task':'transcribe','num_beams':5}
    for f in files:
        x,sr,dur=load(f'{root}/{f}')
        r=asr({'raw':x,'sampling_rate':16000},generate_kwargs=kw)
        res.setdefault(f,{'sr':sr,'dauer_s':round(dur,2)})[mid.split('/')[-1]]=r['text'].strip()
        print(mid.split('/')[-1],f,'|',r['text'].strip(),flush=True)
    del asr; torch.cuda.empty_cache()
json.dump(res,open(out,'w',encoding='utf-8'),ensure_ascii=False,indent=1)
