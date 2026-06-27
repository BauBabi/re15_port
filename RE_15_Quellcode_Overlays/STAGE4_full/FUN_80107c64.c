/* FUN_80107c64 @ 0x80107c64  (Ghidra headless, raw MIPS overlay) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

uint FUN_80107c64(void)

{
  byte bVar1;
  undefined1 uVar2;
  uint uVar3;
  undefined4 uVar4;
  
  bVar1 = *(byte *)(_DAT_800ac784 + 7);
  if (bVar1 != 1) {
    if (bVar1 != 0) {
      return (uint)(bVar1 < 2);
    }
    *(byte *)(_DAT_800ac784 + 0x93) = *(byte *)(_DAT_800ac784 + 0x93) | 1;
    *(undefined1 *)(_DAT_800ac784 + 7) = 1;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 0x1f;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 0xf;
    func_0x80019700(0x2500,(int)*(short *)(_DAT_800ac784 + 0x6a),
                    *(int *)(_DAT_800ac784 + 0x188) + 0x40,&LAB_80119694);
    *(undefined1 *)(_DAT_800ac784 + 0x1b8) = 1;
    if (_DAT_800b0fe0 < 3) {
      uVar2 = *(undefined1 *)(_DAT_800ac784 + 0x1c6);
      uVar4 = 0x800b1038;
    }
    else {
      uVar2 = *(undefined1 *)(_DAT_800ac784 + 0x1c6);
      uVar4 = 0x800b1058;
    }
    func_0x8004ef90(uVar4,uVar2);
  }
  if (*(char *)(_DAT_800ac784 + 0x95) == '\a') {
    uVar3 = func_0x8001af20();
    uVar4 = 8;
    if ((uVar3 & 1) != 0) {
      uVar4 = 5;
    }
    func_0x800453d0(uVar4);
  }
  if (*(char *)(_DAT_800ac784 + 0x95) == '#') {
    func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                    (uint)(byte)(&DAT_80118cac)[*(byte *)(_DAT_800ac784 + 8)] * 0xac +
                    *(int *)(_DAT_800ac784 + 0x188) + 0x40,&LAB_80119694);
  }
  uVar3 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x170),
                          *(undefined4 *)(_DAT_800ac784 + 0x174),0,0x100);
  *(char *)(_DAT_800ac784 + 7) = *(char *)(_DAT_800ac784 + 7) + (char)uVar3;
  return uVar3;
}


