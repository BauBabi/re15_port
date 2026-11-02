import sys, numpy as np, torch
from scipy.io import wavfile
from scipy.signal import resample_poly
from transformers import pipeline
model = sys.argv[1]; files = sys.argv[2:]
asr = pipeline("automatic-speech-recognition", model=model, device=0, torch_dtype=torch.float16)
for f in files:
    r, x = wavfile.read(f)
    if x.dtype == np.int16: x = x.astype(np.float32) / 32768.0
    elif x.dtype == np.int32: x = x.astype(np.float32) / 2147483648.0
    elif x.dtype == np.uint8: x = (x.astype(np.float32) - 128) / 128.0
    else: x = x.astype(np.float32)
    if x.ndim > 1: x = x.mean(axis=1)
    y = resample_poly(x, 16000, r) if r != 16000 else x
    out = asr({"raw": y.astype(np.float32), "sampling_rate": 16000}, return_timestamps=True, generate_kwargs={"num_beams": 5})
    print("==", f, "%.2fs" % (len(x)/r))
    for c in out.get("chunks", []):
        print("   [%s] %s" % (c["timestamp"], c["text"].strip()[:300]))
    sys.stdout.flush()
