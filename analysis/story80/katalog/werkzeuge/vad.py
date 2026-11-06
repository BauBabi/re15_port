"""vad.py: Silero-VAD (torch.hub snakers4/silero-vad) ueber die Tonspuren ton/<name>.wav (16 kHz mono, per ffmpeg -ac 1 -ar 16000).
Schwellen 0.5, 0.3, 0.2; min_speech_duration_ms=150. Ergebnis: daten/vad.json (Schwelle 0.5)."""
import torch, numpy as np, sys, json
from scipy.io import wavfile
W=sys.argv[1]
model, utils = torch.hub.load('snakers4/silero-vad', 'silero_vad', trust_repo=True, verbose=False)
get_ts=utils[0]
res={}
for n in ['B','A','u1','u2','u3','u4','s1','s2','s3','s4']:
    sr,x=wavfile.read(f'{W}/ton/{n}.wav'); x=torch.from_numpy(x.astype(np.float32)/32768)
    for th in (0.5,0.3,0.2):
        ts=get_ts(x,model,sampling_rate=16000,threshold=th,min_speech_duration_ms=150)
        seg=[(round(t['start']/16000,2),round(t['end']/16000,2)) for t in ts]
        if th==0.5: res[n]=seg
        print(n,th,seg)
json.dump(res,open(f'{W}/ton/vad.json','w'),indent=1)
