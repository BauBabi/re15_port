/* FUN_801105c0 @ 0x801105c0  (Ghidra headless overlay RE) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801105c0(void)

{
  short *psVar1;
  int iVar2;
  int iVar3;
  
  if ((int)*(short *)(_DAT_800ac784 + 0x1ba) <= *(int *)(_DAT_800ac784 + 0x38)) {
    *(int *)(_DAT_800ac784 + 0x38) = (int)*(short *)(_DAT_800ac784 + 0x1ba);
    (**(code **)(&DAT_80121070 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
    return;
  }
  if (*(char *)(_DAT_800ac784 + 7) == '\0') {
    *(undefined1 *)(_DAT_800ac784 + 5) = 7;
    *(short *)(_DAT_800ac784 + 0x9c) =
         *(short *)(_DAT_800ac784 + 0x1ba) - *(short *)(_DAT_800ac784 + 0x38);
  }
  iVar2 = _DAT_800ac784;
  iVar3 = *(int *)(_DAT_800ac784 + 0x38);
  *(int *)(_DAT_800ac784 + 0x38) =
       (((int)*(short *)(_DAT_800ac784 + 0x9c) / 0xc) * 0x10000 >> 0x10) + iVar3;
  if (iVar3 == 0) {
    *(undefined1 *)(iVar2 + 6) = 1;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 8;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 0;
    func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),*(undefined4 *)(_DAT_800ac784 + 0x16c),0,
                    0x200);
    iVar2 = _DAT_800ac784;
    psVar1 = (short *)(_DAT_800ac784 + 0xbe);
    *(short *)(_DAT_800ac784 + 0xbc) = *(short *)(_DAT_800ac784 + 0xbc) + 300;
    *(short *)(iVar2 + 0xbe) = *psVar1 + 300;
  }
  else if (iVar3 != 1) {
    FUN_80111754();
    return;
  }
  if (*(char *)(_DAT_800ac784 + 9) == 'B') {
    *(undefined1 *)(_DAT_800ac784 + 9) = 0;
    *(undefined4 *)(_DAT_800ac784 + 4) = 0xc01;
    iVar2 = _DAT_800ac784;
    psVar1 = (short *)(_DAT_800ac784 + 0xbe);
    *(short *)(_DAT_800ac784 + 0xbc) = *(short *)(_DAT_800ac784 + 0xbc) + -300;
    *(short *)(iVar2 + 0xbe) = *psVar1 + -300;
  }
  return;
}


