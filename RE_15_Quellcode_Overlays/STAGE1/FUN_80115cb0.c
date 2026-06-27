/* FUN_80115cb0 @ 0x80115cb0  (Ghidra headless overlay RE) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80115cb0(void)

{
  uint uVar1;
  byte bVar2;
  
  DAT_800acc0c = 1;
  *(undefined1 *)(_DAT_800ac784 + 0x6e) = 0;
  *(undefined1 *)((int)_DAT_800ac784 + 0x1b9) = 0;
  *(undefined1 *)((int)_DAT_800ac784 + 0x93) = 0;
  *(undefined2 *)((int)_DAT_800ac784 + 0x9a) = 100;
  _DAT_800ac784[0x1e] = _DAT_80121264;
  *(undefined1 *)(_DAT_800ac784 + 0x25) = 0;
  *(undefined1 *)((int)_DAT_800ac784 + 0x95) = 0;
  *(undefined1 *)((int)_DAT_800ac784 + 0x8f) = 0;
  func_0x8001f314(_DAT_800ac784[0x21],_DAT_800ac784[0x5b],0,0x200);
  func_0x8001af5c(0,0,600,600,_DAT_800ac784 + 0x2c,0xa0a0a0);
  bVar2 = 0;
  uVar1 = 0;
  do {
    bVar2 = bVar2 + 1;
    _DAT_800ac784[uVar1 + 0x74] = 0;
    uVar1 = (uint)bVar2;
  } while (bVar2 < 8);
  *(uint *)_DAT_800ac784[0x62] = *(uint *)_DAT_800ac784[0x62] & 0xfffffffe;
  *(short *)(_DAT_800ac784 + 0x76) = (short)_DAT_800ac784[0xd];
  *(short *)((int)_DAT_800ac784 + 0x1da) = (short)_DAT_800ac784[0xf];
  *(short *)((int)_DAT_800ac784 + 0x1d6) = (short)_DAT_800ac784[0xe];
  *_DAT_800ac784 = *_DAT_800ac784 & 0x1fffffff;
  *_DAT_800ac784 = *_DAT_800ac784 | 0x40000000;
  *_DAT_800ac784 = *_DAT_800ac784 | 8;
  _DAT_800ac784[1] = 1;
  *(byte *)((int)_DAT_800ac784 + 5) = *(byte *)((int)_DAT_800ac784 + 9) & 0x7f;
  if ((*(byte *)((int)_DAT_800ac784 + 9) & 0x80) == 0) {
    func_0x80019700(0x9031800,(int)*(short *)((int)_DAT_800ac784 + 0x6a),_DAT_800ac784[0x62] + 0x40,
                    0x80121248);
  }
  return;
}


