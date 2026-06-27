/* FUN_80115d94 @ 0x80115d94  (Ghidra headless overlay RE) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80115d94(void)

{
  uint uVar1;
  byte bVar2;
  
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


