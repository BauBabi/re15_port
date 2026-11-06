import os, sys, json, numpy as np, torch
from scipy.io import wavfile
W=sys.argv[1]; model_id=sys.argv[2]; jobs=sys.argv[3:]
from transformers import WhisperProcessor, WhisperForConditionalGeneration
proc=WhisperProcessor.from_pretrained(model_id)
model=WhisperForConditionalGeneration.from_pretrained(model_id,torch_dtype=torch.float16).to('cuda').eval()
out={}
for job in jobs:
    name,t0,t1,lang=job.split(':')
    sr,x=wavfile.read(f'{W}/ton/{name}.wav'); x=x.astype(np.float32)/32768
    seg=x[int(float(t0)*sr):int(float(t1)*sr)]
    feats=proc(seg,sampling_rate=16000,return_tensors='pt').input_features.to('cuda',torch.float16)
    res={}
    # language detection
    with torch.no_grad():
        enc=model.model.encoder(feats)
        dec_in=torch.tensor([[model.config.decoder_start_token_id]],device='cuda')
        logits=model(encoder_outputs=enc,decoder_input_ids=dec_in).logits[0,-1].float()
    tok=proc.tokenizer
    lang_ids={l:tok.convert_tokens_to_ids(f'<|{l}|>') for l in ['ja','en','zh','ko','de','fr','es','it']}
    lp=torch.softmax(torch.stack([logits[i] for i in lang_ids.values()]),0)
    res['lang']={l:round(float(p),3) for l,p in zip(lang_ids,lp)}
    nospeech=tok.convert_tokens_to_ids('<|nospeech|>')
    if nospeech is None or nospeech==tok.unk_token_id: nospeech=tok.convert_tokens_to_ids('<|nocaptions|>')
    for L in (['ja','en'] if lang=='both' else [lang]):
        with torch.no_grad():
            pre=torch.tensor([[model.config.decoder_start_token_id,tok.convert_tokens_to_ids(f'<|{L}|>'),tok.convert_tokens_to_ids('<|transcribe|>')]],device='cuda')
            lg=model(encoder_outputs=enc,decoder_input_ids=pre[:,:1]).logits[0,-1].float()
            nsp=float(torch.softmax(lg,0)[nospeech])
            g=model.generate(feats,language=L,task='transcribe',num_beams=5,return_timestamps=True)
        txt=proc.batch_decode(g,skip_special_tokens=False,decode_with_timestamps=True)[0]
        res[L]={'text':txt,'no_speech_prob':round(nsp,3)}
    out[job]=res
    print(job, json.dumps(res,ensure_ascii=False), flush=True)
json.dump(out,open(f'{W}/ton/whisper_{model_id.split("/")[-1]}.json','w',encoding='utf-8'),ensure_ascii=False,indent=1)
