/* FUN_80115e24 @ 0x80115e24  (Ghidra headless overlay RE) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80115e24(void)

{
  int in_v1;
  
  *(undefined2 *)(in_v1 + 0x1d6) = *(undefined2 *)(in_v1 + 0x38);
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


