#!/usr/bin/env python3
"""Gleiche Teile-Zuordnung fuer alle drei Rohbauten. streng: Oberkoerper ohne Bein. bein: wie A und C es in form.json halten."""
import json
import os
from pathlib import Path

H = Path(__file__).resolve().parent
F = H.parent
(H / 'mess').mkdir(exist_ok=True)
FUB = [0, 7, 10, 11, 16, 21, 22, 23]
for r, o in (('A', 'rohbau_a'), ('B', 'rohbau_b'), ('C', 'rohbau_c')):
    j = json.load(open(F / o / 'form.json', encoding='utf-8'))
    j['emd'] = (F / o / 'FORM.EMD').as_posix()
    j['tim'] = (F / o / 'FORM.TIM').as_posix()
    j['name'] = r
    j['teile'] = dict(tail=[1, 2, 3, 4, 5, 6], left_leg=[8, 24, 25], right_leg=[9, 26, 27], right_arm=[16, 17, 18, 19, 20, 29, 31],
                      part_of_Body=[0, 7, 10], face_and_upper_body=FUB, left_arm_front=[12, 13, 14, 15, 28, 30], right_arm_front=[17, 18, 19, 20, 29, 31])
    json.dump(j, open(H / 'mess' / (r + '_streng.json'), 'w', encoding='utf-8'), indent=1)
    j['teile']['face_and_upper_body'] = FUB + [9, 26, 27]
    json.dump(j, open(H / 'mess' / (r + '_bein.json'), 'w', encoding='utf-8'), indent=1)
