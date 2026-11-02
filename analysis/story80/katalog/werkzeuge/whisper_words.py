import sys, json, numpy as np, torch
from scipy.io import wavfile
from transformers import pipeline
W=sys.argv[1]; mid=sys.argv[2]; jobs=sys.argv[3:]
asr=pipeline('automatic-speech-recognition',model=mid,torch_dtype=torch.float16,device='cuda')
out={}
for job in jobs:
    name,t0,t1=job.split(':')[:3]
    sr,x=wavfile.read(f'{W}/ton/{name}.wav'); x=x.astype(np.float32)/32768
    seg=x[int(float(t0)*sr):int(float(t1)*sr)]
    r=asr({'raw':seg,'sampling_rate':16000},return_timestamps='word',generate_kwargs=({'num_beams':5} if mid.endswith('.en') else {'language':'en','task':'transcribe','num_beams':5}))
    words=[(round(c['timestamp'][0]+float(t0),2) if c['timestamp'][0] is not None else None,round(c['timestamp'][1]+float(t0),2) if c['timestamp'][1] is not None else None,c['text']) for c in r['chunks']]
    out[job]={'text':r['text'],'words':words}
    print(job,'|',r['text']); print('   ',' '.join(f'{w}[{a}-{b}]' for a,b,w in words),flush=True)
json.dump(out,open(f'{W}/ton/words_{mid.split("/")[-1]}.json','w',encoding='utf-8'),indent=1,ensure_ascii=False)
