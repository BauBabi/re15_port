/* FUN_80115dc8 @ 0x80115dc8  (Ghidra headless overlay RE) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80115dc8(void)

{
  int in_v0;
  uint *puVar1;
  
  puVar1 = *(uint **)(*(int *)(in_v0 + -0x387c) + 0x188);
  *puVar1 = *puVar1 & 0xfffffffe;
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


