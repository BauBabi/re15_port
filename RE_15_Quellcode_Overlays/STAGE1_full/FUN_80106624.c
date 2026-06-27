/* FUN_80106624 @ 0x80106624  (Ghidra headless, raw MIPS overlay) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80106624(void)

{
  uint uVar1;
  int iVar2;
  
  if (*(char *)(_DAT_800ac784 + 7) == '\0') {
    *(undefined1 *)(_DAT_800ac784 + 7) = 1;
    *(byte *)(_DAT_800ac784 + 0x9f) = *(byte *)(_DAT_800ac784 + 0x93) >> 7;
    *(char *)(_DAT_800ac784 + 0x94) = '\x10' - *(char *)(_DAT_800ac784 + 0x9f);
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 3;
    *(undefined2 *)(_DAT_800ac784 + 0x8c) = 100;
    *(undefined1 *)(_DAT_800ac784 + 0x9e) = 0xc;
    *(byte *)(_DAT_800ac784 + 0x93) = *(byte *)(_DAT_800ac784 + 0x93) | 1;
    func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                    *(int *)(_DAT_800ac784 + 0x188) + 0x40,&LAB_8011fe84);
    func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                    (uint)(byte)(&DAT_8011f784)[*(byte *)(_DAT_800ac784 + 8)] * 0xac +
                    *(int *)(_DAT_800ac784 + 0x188) + 0x40,&LAB_8011fe84);
    uVar1 = func_0x8001af20();
    if ((uVar1 & 1) == 0) {
      func_0x800453d0(6);
    }
  }
  iVar2 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x170),
                          *(undefined4 *)(_DAT_800ac784 + 0x174),0,0x400);
  if (iVar2 != 0) {
    *(undefined4 *)(_DAT_800ac784 + 4) = 0x201;
    *(byte *)(_DAT_800ac784 + 0x93) = *(byte *)(_DAT_800ac784 + 0x93) & 0xfe;
    uVar1 = func_0x8001af20();
    if ((uVar1 & 1) == 0) {
      *(undefined1 *)(_DAT_800ac784 + 5) = 8;
    }
    iVar2 = func_0x8001a780(&DAT_800aca54);
    if (iVar2 != 0) {
      *(undefined1 *)(_DAT_800ac784 + 5) = 7;
    }
  }
  func_0x800245d8(*(char *)(_DAT_800ac784 + 0x9f) * -0x800 + 0x800);
  *(short *)(_DAT_800ac784 + 0x8c) =
       *(short *)(_DAT_800ac784 + 0x8c) - (short)*(char *)(_DAT_800ac784 + 0x9e);
  *(char *)(_DAT_800ac784 + 0x9e) = *(char *)(_DAT_800ac784 + 0x9e) + -1;
  if (*(char *)(_DAT_800ac784 + 0x9e) < '\0') {
    *(undefined1 *)(_DAT_800ac784 + 0x9e) = 0;
  }
  return;
}


