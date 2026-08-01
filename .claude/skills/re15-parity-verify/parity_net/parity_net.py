#!/usr/bin/env python3
"""
parity_net -- content-based frame matcher for re15-parity-verify.

Problem (SKILL.md section 4): DuckStation runs at VARIABLE fps, so PSX captures
and the deterministic 30-fps port series can NOT be aligned by time. Screen-by-
screen parity checks therefore need a TIMING-INDEPENDENT, content-based frame
match that is robust to the renderer gap (PSX: 15-bit RGB555 + ordered dither +
MDEC backgrounds; port: clean 24-bit software render).

This tool trains a small CNN embedding (contrastive / NT-Xent, self-supervised)
whose augmentations SIMULATE that renderer gap (RGB555 quantize + Bayer dither +
noise + pixelation + color jitter), so the same game state lands at the same
point in embedding space regardless of which side rendered it.

NOTE (RE discipline): a high cosine score is "sieht richtig aus" evidence ONLY.
It aligns/triages frames for screen-by-screen comparison; it is NOT byte-true
proof of a mechanism. Divergence candidates it flags must still be pinned to
disassembly addresses / RAM values per CLAUDE.md.

Subcommands:
  build-data   assemble data/{port,psx}/ from shots/ + stage_saves/
  train        contrastive training -> parity_net.pt
  eval         self-test: PSX-ified queries vs clean gallery (top-1 retrieval)
  match        match query frames (PSX) against a gallery (port series):
               CSV report + side-by-side sheet + optional monotonic alignment

Typical use:
  python parity_net.py build-data
  python parity_net.py train
  python parity_net.py match --queries "stage_saves/mzd_room1140_verify/ORIG_wake" \
                             --gallery "shots/series_f*.bmp" --sheet
"""
import argparse
import csv
import glob
import math
import os
import sys
from pathlib import Path

import numpy as np
from PIL import Image, ImageDraw

SCRIPT_DIR = Path(__file__).resolve().parent
REPO = SCRIPT_DIR.parents[3]          # .../repo/.claude/skills/<skill>/parity_net
DATA_DIR = SCRIPT_DIR / "data"
CKPT_DEFAULT = SCRIPT_DIR / "parity_net.pt"
OUT_DIR = SCRIPT_DIR / "out"

FRAME_W, FRAME_H = 320, 240           # native PSX/port frame size (dataset storage)
NET_W, NET_H = 160, 120               # network input size
# Framebuffer window inside the 1024x512 re15_ss.py VRAM dumps. x=440 measured
# against stage_saves/mzd_stage1_briefing.png + mzd_demo/* (double buffer y=0/240).
VRAM_FB_X, VRAM_FB_YS = 440, (0, 240)


# --------------------------------------------------------------------------
# dataset building
# --------------------------------------------------------------------------

def _save(img, path):
    path.parent.mkdir(parents=True, exist_ok=True)
    img.convert("RGB").resize((FRAME_W, FRAME_H), Image.LANCZOS).save(path)


def _mask_label(tile):
    """Blank the burned-in '#N' / 'oNN' capture label (top-left corner) so the
    net cannot cheat by reading it. Filled with the tile median color."""
    a = np.asarray(tile.convert("RGB")).copy()
    h, w = a.shape[:2]
    mh, mw = max(1, int(h * 0.09)), max(1, int(w * 0.24))
    a[:mh, :mw] = np.median(a.reshape(-1, 3), axis=0).astype(a.dtype)
    return Image.fromarray(a)


def _slice_sheet(path, cols, rows, prefix, out_dir, min_std=8.0):
    img = Image.open(path).convert("RGB")
    tw, th = img.width // cols, img.height // rows
    n = 0
    for r in range(rows):
        for c in range(cols):
            tile = img.crop((c * tw, r * th, (c + 1) * tw, (r + 1) * th))
            if np.asarray(tile).std() < min_std:   # skip empty/black cells
                continue
            _save(_mask_label(tile), out_dir / f"{prefix}{r * cols + c:02d}.png")
            n += 1
    return n


def _crop_vram(path):
    """Pick the live double-buffer (y=0 or y=240) at the fixed x window; the
    stale buffer scores worse on gradient-vs-structure."""
    a = np.asarray(Image.open(path).convert("RGB"), dtype=np.float32)
    if a.shape[:2] != (512, 1024):
        return None
    best = None
    for y in VRAM_FB_YS:
        win = a[y:y + FRAME_H, VRAM_FB_X:VRAM_FB_X + FRAME_W]
        score = np.abs(np.diff(win, axis=1)).mean() / max(win.std(), 1.0)
        if best is None or score < best[0]:
            best = (score, win)
    return Image.fromarray(best[1].astype(np.uint8))


def cmd_build_data(args):
    port_dir, psx_dir = DATA_DIR / "port", DATA_DIR / "psx"
    n_port = n_psx = 0

    for f in sorted(glob.glob(str(REPO / "shots" / "series_f*.bmp"))):
        _save(Image.open(f), port_dir / (Path(f).stem + ".png")); n_port += 1
    for f in sorted(glob.glob(str(REPO / "shots" / "m_*.png"))):
        _save(Image.open(f), port_dir / (Path(f).stem + ".png")); n_port += 1

    rv = REPO / "stage_saves" / "mzd_room1140_verify"
    n_psx += _slice_sheet(rv / "ORIG_wake.png", 4, 4, "wake", psx_dir)
    _save(Image.open(rv / "ORIG_grab_death.png"), psx_dir / "grab_death.png"); n_psx += 1
    n_psx += _slice_sheet(REPO / "stage_saves" / "mzd_preintro_verify" / "contact_ORIGINAL.png",
                          5, 6, "intro", psx_dir)
    n_psx += _slice_sheet(REPO / "shots" / "attack_strip.png", 6, 1, "attack", psx_dir)
    # port rendering of the SAME intro montage -> genuine cross-domain pairs (GROUPS)
    n_port += _slice_sheet(REPO / "stage_saves" / "mzd_preintro_verify" / "port_full_sequence.png",
                           5, 5, "pintro", port_dir)

    vram_files = sorted(glob.glob(str(REPO / "stage_saves" / "*.png"))) + \
                 sorted(glob.glob(str(REPO / "stage_saves" / "mzd_demo" / "*.png")))
    for f in vram_files:
        if Path(f).name == "vram_view.png":     # viewer output, different layout
            continue
        crop = _crop_vram(f)
        if crop is not None:
            _save(crop, psx_dir / ("vram_" + Path(f).stem + ".png")); n_psx += 1

    print(f"dataset: {n_port} port frames -> {port_dir}")
    print(f"dataset: {n_psx} psx frames  -> {psx_dir}")


# --------------------------------------------------------------------------
# cross-domain positive groups (content-verified by inspection, 2026-07-03).
# Files in one group show the SAME game state rendered by BOTH sides
# (psx/intro* = contact_ORIGINAL tiles oNN; port/pintro* = port_full_sequence
# tiles at frame NN*100; room1140: VRAM spawn dumps vs port series spawn).
# This is what actually bridges the renderer gap: without them, instance
# discrimination PUSHES true psx<->port pairs apart (they are 'different
# instances'), and real cross-domain matching fails.
# --------------------------------------------------------------------------

GROUPS = [
    # ROOM1140 spawn, door cam, Leon at/near (-7600,-17600)
    ["psx/vram_mzd_stage1_briefing", "psx/vram_mzd_stage1_briefing_live",
     "psx/wake00", "psx/wake01", "port/series_f00100", "port/series_f00110",
     "port/series_f00120"],
    # intro montage slides (text-verified)
    ["psx/intro09", "psx/intro10", "psx/intro11",              # "A bizarre incident..."
     "port/pintro04", "port/pintro05", "port/pintro06", "port/pintro07"],
    ["psx/intro12", "psx/intro13", "psx/intro14",              # "It was later revealed..."
     "port/pintro08", "port/pintro09"],
    ["psx/intro15", "psx/intro16",                             # "a mutagenic toxin..."
     "port/pintro12", "port/pintro13"],
    ["psx/intro17",                                            # aerial, no text
     "port/pintro10", "port/pintro11", "port/pintro14"],
    ["psx/intro18",                                            # S.T.A.R.S. badge
     "port/pintro15", "port/pintro16", "port/pintro17"],
    ["psx/intro19", "psx/intro20",                             # machinery scene
     "port/pintro18"],
    ["psx/intro21",                                            # helicopter + explosion
     "port/pintro19", "port/pintro20", "port/pintro21"],
    ["psx/intro22", "psx/intro23",                             # squad group photo
     "port/pintro22"],
]


def group_labels(files):
    """Return an int group id per file; ungrouped files get unique ids."""
    key = {}
    for gi, grp in enumerate(GROUPS):
        for name in grp:
            key[name.replace("/", os.sep)] = gi
    labels, nxt = [], len(GROUPS)
    for f in files:
        rel = os.sep.join([Path(f).parent.name, Path(f).stem])
        if rel in key:
            labels.append(key[rel])
        else:
            labels.append(nxt); nxt += 1
    return labels


# --------------------------------------------------------------------------
# model
# --------------------------------------------------------------------------

def _torch():
    import torch
    import torch.nn as nn
    import torch.nn.functional as F
    return torch, nn, F


def build_encoder(emb_dim=128):
    torch, nn, _ = _torch()

    def block(cin, cout):
        return nn.Sequential(nn.Conv2d(cin, cout, 3, stride=2, padding=1),
                             nn.BatchNorm2d(cout), nn.ReLU(inplace=True))

    class Encoder(nn.Module):
        def __init__(self):
            super().__init__()
            self.body = nn.Sequential(block(3, 32), block(32, 64), block(64, 128),
                                      block(128, 256), block(256, 256),
                                      nn.AdaptiveAvgPool2d(1), nn.Flatten())
            self.head = nn.Linear(256, emb_dim)

        def forward(self, x):
            z = self.head(self.body(x))
            return torch.nn.functional.normalize(z, dim=1)

    return Encoder()


def load_encoder(ckpt_path, device):
    torch, _, _ = _torch()
    ck = torch.load(ckpt_path, map_location=device, weights_only=False)
    net = build_encoder(ck["emb_dim"]).to(device)
    net.load_state_dict(ck["state_dict"])
    net.eval()
    return net


# --------------------------------------------------------------------------
# GPU augmentations -- simulate the PSX<->port renderer gap
# --------------------------------------------------------------------------

_BAYER4 = np.array([[0, 8, 2, 10], [12, 4, 14, 6],
                    [3, 11, 1, 9], [15, 7, 13, 5]], dtype=np.float32) / 16.0


def _rand(torch, n, a, b, dev):
    return torch.rand(n, 1, 1, 1, device=dev) * (b - a) + a


def aug_geom(x, out_hw=(NET_H, NET_W)):
    """Random zoom-crop + shift via one batched affine grid_sample."""
    torch, _, F = _torch()
    n, dev = x.size(0), x.device
    s = torch.rand(n, device=dev) * 0.22 + 0.78
    tx = (1 - s) * (torch.rand(n, device=dev) * 2 - 1)
    ty = (1 - s) * (torch.rand(n, device=dev) * 2 - 1)
    theta = torch.zeros(n, 2, 3, device=dev)
    theta[:, 0, 0], theta[:, 1, 1] = s, s
    theta[:, 0, 2], theta[:, 1, 2] = tx, ty
    grid = F.affine_grid(theta, (n, 3, out_hw[0], out_hw[1]), align_corners=False)
    return F.grid_sample(x, grid, mode="bilinear",
                         padding_mode="reflection", align_corners=False)


def aug_color(x, strength):
    torch, _, _ = _torch()
    n, dev = x.size(0), x.device
    x = x * _rand(torch, n, 1 - 0.25 * strength, 1 + 0.25 * strength, dev)       # brightness
    mean = x.mean(dim=(2, 3), keepdim=True)
    x = (x - mean) * _rand(torch, n, 1 - 0.3 * strength, 1 + 0.3 * strength, dev) + mean  # contrast
    gains = 1 + (torch.rand(n, 3, 1, 1, device=dev) * 2 - 1) * 0.12 * strength   # channel tint
    x = x * gains
    x = x.clamp(1e-4, 1).pow(_rand(torch, n, 1 - 0.3 * strength, 1 + 0.3 * strength, dev))  # gamma
    return x.clamp(0, 1)


def aug_psxify(x):
    """PSX look: pixelation, RGB555 quantization with 4x4 Bayer ordered dither,
    sensor/MDEC-ish noise. Applied AFTER geometry, on the net-input resolution."""
    torch, _, F = _torch()
    n, dev = x.size(0), x.device
    x = aug_color(x, strength=1.0)
    scale = float(np.random.uniform(0.65, 1.0))                  # pixelate
    h, w = x.shape[2:]
    x = F.interpolate(F.interpolate(x, scale_factor=scale, mode="nearest"),
                      size=(h, w), mode="nearest")
    bayer = torch.from_numpy(_BAYER4).to(dev)
    bayer = bayer.repeat(math.ceil(h / 4), math.ceil(w / 4))[:h, :w]
    x = torch.floor(x * 31 + bayer) / 31                         # RGB555 + dither
    x = x + torch.randn_like(x) * _rand(torch, n, 0.005, 0.03, dev)
    return x.clamp(0, 1)


def aug_portify(x):
    """Port look: clean render, mild jitter, occasional slight blur."""
    torch, _, F = _torch()
    n, dev = x.size(0), x.device
    x = aug_color(x, strength=0.5)
    if np.random.rand() < 0.5:
        x = F.avg_pool2d(x, 3, stride=1, padding=1)
    x = x + torch.randn_like(x) * _rand(torch, n, 0.0, 0.01, dev)
    return x.clamp(0, 1)


# --------------------------------------------------------------------------
# data loading helpers
# --------------------------------------------------------------------------

def _expand_inputs(spec):
    """Accept a dir, a glob, or a comma list of either; return sorted files."""
    files = []
    for part in spec.split(","):
        p = Path(part)
        if p.is_dir():
            for ext in ("*.png", "*.bmp", "*.jpg"):
                files += glob.glob(str(p / ext))
        else:
            files += glob.glob(part)
    files = sorted(dict.fromkeys(files))
    if not files:
        sys.exit(f"no images match: {spec}")
    return files


def load_images(files, device, size=(FRAME_W, FRAME_H)):
    torch, _, _ = _torch()
    arrs = []
    for f in files:
        im = Image.open(f).convert("RGB").resize(size, Image.LANCZOS)
        arrs.append(np.asarray(im, dtype=np.float32) / 255.0)
    x = torch.from_numpy(np.stack(arrs)).permute(0, 3, 1, 2)
    return x.to(device)


def embed_clean(net, x, device, batch=64):
    """Embed frames without augmentation (resize to net input only)."""
    torch, _, F = _torch()
    outs = []
    with torch.no_grad():
        for i in range(0, x.size(0), batch):
            xb = F.interpolate(x[i:i + batch], size=(NET_H, NET_W),
                               mode="bilinear", align_corners=False)
            outs.append(net(xb))
    return torch.cat(outs)


# --------------------------------------------------------------------------
# training
# --------------------------------------------------------------------------

def cmd_train(args):
    torch, _, F = _torch()
    torch.manual_seed(args.seed); np.random.seed(args.seed)
    device = "cuda" if torch.cuda.is_available() else "cpu"

    files = sorted(glob.glob(str(DATA_DIR / "*" / "*.png")))
    if not files:
        sys.exit("dataset empty -- run: python parity_net.py build-data")
    x = load_images(files, device)
    n = x.size(0)
    glab = torch.tensor(group_labels(files), device=device)
    n_grouped = sum(1 for g in glab.tolist() if glab.tolist().count(g) > 1)
    print(f"training on {n} frames ({device}), {args.epochs} epochs, "
          f"{len(GROUPS)} cross-domain groups ({n_grouped} grouped frames)")

    net = build_encoder(args.emb_dim).to(device)
    opt = torch.optim.Adam(net.parameters(), lr=args.lr)
    sched = torch.optim.lr_scheduler.CosineAnnealingLR(opt, args.epochs, eta_min=args.lr * 0.1)
    tau = args.tau

    net.train()
    for ep in range(1, args.epochs + 1):
        # full batch: keeps every cross-domain group intact in every step
        za = net(aug_portify(aug_geom(x)))      # view A: port-like
        zb = net(aug_psxify(aug_geom(x)))       # view B: psx-like
        z = torch.cat([za, zb])
        g2 = torch.cat([glab, glab])
        sim = z @ z.t() / tau
        eye = torch.eye(2 * n, dtype=torch.bool, device=device)
        # SupCon (Khosla et al. 2020): positives = same content group
        # (the aug twin always included); negatives = the rest.
        pos = (g2.unsqueeze(0) == g2.unsqueeze(1)) & ~eye
        logprob = sim - torch.logsumexp(sim.masked_fill(eye, -1e4), dim=1, keepdim=True)
        loss = -(logprob * pos).sum(1).div(pos.sum(1).clamp(min=1)).mean()
        opt.zero_grad(); loss.backward(); opt.step()
        sched.step()
        if ep % 100 == 0 or ep == 1:
            print(f"  epoch {ep:4d}  loss {loss.item():.4f}")

    ck = {"state_dict": {k: v.cpu() for k, v in net.state_dict().items()},
          "emb_dim": args.emb_dim, "net_hw": (NET_H, NET_W)}
    torch.save(ck, args.ckpt)
    print(f"saved {args.ckpt}")
    _run_eval(args.ckpt, rounds=3, seed=args.seed)


# --------------------------------------------------------------------------
# evaluation: psx-ified queries against the clean gallery
# --------------------------------------------------------------------------

def _run_eval(ckpt, rounds, seed):
    torch, _, _ = _torch()
    torch.manual_seed(seed); np.random.seed(seed)
    device = "cuda" if torch.cuda.is_available() else "cpu"
    net = load_encoder(ckpt, device)

    files = sorted(glob.glob(str(DATA_DIR / "*" / "*.png")))
    x = load_images(files, device)
    gal = embed_clean(net, x, device)

    glab = group_labels(files)
    hits = tot = 0
    with torch.no_grad():
        for _ in range(rounds):
            q = net(aug_psxify(aug_geom(x)))
            sim = q @ gal.t()
            top1 = sim.argmax(dim=1)
            for i in range(x.size(0)):
                j = top1[i].item()
                # same group or +-1 series neighbor counts (near-identical)
                ok = j == i or glab[i] == glab[j] or \
                     (Path(files[i]).parent == Path(files[j]).parent and abs(i - j) <= 1)
                hits += ok; tot += 1
    print(f"eval: psx-ified retrieval top-1 (group / +-1 neighbor): "
          f"{hits}/{tot} = {hits / tot:.1%}")

    # REAL cross-domain check: grouped PSX frames must retrieve a port-domain
    # frame of the SAME group from a port-only gallery.
    port_idx = [i for i, f in enumerate(files) if Path(f).parent.name == "port"]
    psx_q = [i for i, f in enumerate(files) if Path(f).parent.name == "psx"
             and glab.count(glab[i]) > 1]
    if port_idx and psx_q:
        gp = gal[port_idx]
        xhits = 0
        with torch.no_grad():
            sim = gal[psx_q] @ gp.t()
            top1 = sim.argmax(dim=1)
        for k, i in enumerate(psx_q):
            j = port_idx[top1[k].item()]
            xhits += glab[i] == glab[j]
        print(f"eval: REAL cross-domain (psx anchor -> port gallery, same group): "
              f"{xhits}/{len(psx_q)} = {xhits / len(psx_q):.1%}")
    return hits / tot


def cmd_eval(args):
    _run_eval(args.ckpt, rounds=args.rounds, seed=args.seed)


# --------------------------------------------------------------------------
# matching / alignment report
# --------------------------------------------------------------------------

def _monotonic_align(sim):
    """DTW-style monotonic path (queries and gallery both temporal series):
    every query row picks a gallery column >= the previous pick's column."""
    nq, ng = sim.shape
    cost = 1.0 - sim
    dp = np.full((nq, ng), np.inf, dtype=np.float64)
    back = np.zeros((nq, ng), dtype=np.int64)
    dp[0] = cost[0]
    for i in range(1, nq):
        run_min = np.minimum.accumulate(dp[i - 1])
        run_arg = np.zeros(ng, dtype=np.int64)
        best = 0
        for j in range(1, ng):
            if dp[i - 1, j] < dp[i - 1, best]:
                best = j
            run_arg[j] = best
        dp[i] = cost[i] + run_min
        back[i] = run_arg
    j = int(dp[-1].argmin())
    path = [j]
    for i in range(nq - 1, 0, -1):
        j = int(back[i][path[-1]])
        path.append(j)
    return list(reversed(path))


def cmd_match(args):
    torch, _, _ = _torch()
    device = "cuda" if torch.cuda.is_available() else "cpu"
    net = load_encoder(args.ckpt, device)

    qf = _expand_inputs(args.queries)
    gf = _expand_inputs(args.gallery)
    xq = load_images(qf, device)
    xg = load_images(gf, device)
    zq = embed_clean(net, xq, device)
    zg = embed_clean(net, xg, device)
    sim = (zq @ zg.t()).cpu().numpy()

    picks = _monotonic_align(sim) if args.align else sim.argmax(axis=1).tolist()

    OUT_DIR.mkdir(parents=True, exist_ok=True)
    csv_path = OUT_DIR / (args.name + ".csv")
    with open(csv_path, "w", newline="") as fh:
        w = csv.writer(fh)
        w.writerow(["query", "best_gallery_match", "cosine", "verdict"])
        for i, f in enumerate(qf):
            j = picks[i]
            score = float(sim[i, j])
            verdict = "MATCH" if score >= args.threshold else "CHECK-DIVERGENCE"
            w.writerow([Path(f).name, Path(gf[j]).name, f"{score:.4f}", verdict])
            print(f"  {Path(f).name:28s} -> {Path(gf[j]).name:24s} "
                  f"cos={score:.3f}  {verdict}")
    print(f"report: {csv_path}")

    if args.sheet:
        tw, th, pad = 320, 240, 18
        sheet = Image.new("RGB", (2 * tw + 8, len(qf) * (th + pad)), (24, 24, 24))
        drw = ImageDraw.Draw(sheet)
        for i, f in enumerate(qf):
            j = picks[i]
            y = i * (th + pad)
            sheet.paste(Image.open(f).convert("RGB").resize((tw, th)), (0, y))
            sheet.paste(Image.open(gf[j]).convert("RGB").resize((tw, th)), (tw + 8, y))
            drw.text((4, y + th + 3),
                     f"{Path(f).name}  ->  {Path(gf[j]).name}   cos={sim[i, j]:.3f}",
                     fill=(255, 255, 128))
        sheet_path = OUT_DIR / (args.name + "_sheet.png")
        sheet.save(sheet_path)
        print(f"sheet:  {sheet_path}")


# --------------------------------------------------------------------------

def main():
    ap = argparse.ArgumentParser(description=__doc__,
                                 formatter_class=argparse.RawDescriptionHelpFormatter)
    sub = ap.add_subparsers(dest="cmd", required=True)

    sub.add_parser("build-data", help="assemble data/ from shots/ + stage_saves/")

    tp = sub.add_parser("train", help="contrastive training")
    tp.add_argument("--epochs", type=int, default=1500)
    tp.add_argument("--batch", type=int, default=128)
    tp.add_argument("--lr", type=float, default=1e-3)
    tp.add_argument("--tau", type=float, default=0.2)
    tp.add_argument("--emb-dim", type=int, default=128)
    tp.add_argument("--seed", type=int, default=1337)
    tp.add_argument("--ckpt", type=Path, default=CKPT_DEFAULT)

    ep = sub.add_parser("eval", help="augmented-retrieval self-test")
    ep.add_argument("--rounds", type=int, default=5)
    ep.add_argument("--seed", type=int, default=7)
    ep.add_argument("--ckpt", type=Path, default=CKPT_DEFAULT)

    mp = sub.add_parser("match", help="match queries (PSX) vs gallery (port)")
    mp.add_argument("--queries", required=True, help="dir | glob | comma list")
    mp.add_argument("--gallery", required=True, help="dir | glob | comma list")
    mp.add_argument("--ckpt", type=Path, default=CKPT_DEFAULT)
    mp.add_argument("--threshold", type=float, default=0.70)
    mp.add_argument("--align", action="store_true",
                    help="monotonic series-to-series alignment (both temporal)")
    mp.add_argument("--sheet", action="store_true", help="write side-by-side PNG")
    mp.add_argument("--name", default="match", help="output basename in out/")

    args = ap.parse_args()
    {"build-data": cmd_build_data, "train": cmd_train,
     "eval": cmd_eval, "match": cmd_match}[args.cmd](args)


if __name__ == "__main__":
    main()
