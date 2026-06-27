/* FUN_801118a8 @ 0x801118a8  (Ghidra headless, raw MIPS overlay) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801118a8(void)

{
  byte bVar1;
  short sVar2;
  undefined2 uVar3;
  ushort uVar4;
  undefined4 uVar5;
  
  if (*(byte *)(_DAT_800ac784 + 9) < 3) {
    bVar1 = *(byte *)(_DAT_800ac784 + 0x90);
    if ((((bVar1 & 3) != 0) && (*(byte *)(_DAT_800ac784 + 9) < 2)) &&
       ((((bVar1 & 0xf0) * 0x10 - (int)*(short *)(_DAT_800ac784 + 0x6a)) + 0x200 & 0xfff) < 0x400))
    {
      *(ushort *)(_DAT_800ac784 + 0x1ec) = bVar1 & 1;
      *(undefined1 *)(_DAT_800ac784 + 0x9f) = *(undefined1 *)(_DAT_800ac784 + 0x90);
      *(undefined2 *)(_DAT_800ac784 + 4) = 0x101;
      uVar3 = 9;
      goto LAB_80111c48;
    }
    *(undefined2 *)(_DAT_800ac784 + 0x1e2) = 0;
    if (*(char *)(_DAT_800ac784 + 9) == '\x02') {
      *(undefined2 *)(_DAT_800ac784 + 0x1e2) = 0x800;
    }
    if (((*(ushort *)(*(int *)(_DAT_800ac784 + 0x1b4) + 10) & 0xc) != 0) &&
       ((((*(byte *)(_DAT_800ac784 + 0x90) & 0xf0) * 0x10 -
         ((int)*(short *)(_DAT_800ac784 + 0x6a) + (int)*(short *)(_DAT_800ac784 + 0x1e2))) + 0x200 &
        0xfff) < 0x400)) {
      *(byte *)(_DAT_800ac784 + 0x9f) = *(byte *)(_DAT_800ac784 + 0x90);
      *(undefined2 *)(_DAT_800ac784 + 4) = 0x101;
      *(undefined2 *)(_DAT_800ac784 + 6) = 0xc;
      *(undefined2 *)(_DAT_800ac784 + 0x1ee) = 0;
      *(undefined2 *)(_DAT_800ac784 + 0x1e8) = *(undefined2 *)(*(int *)(_DAT_800ac784 + 0x1b4) + 6);
      *(short *)(_DAT_800ac784 + 0x1ea) =
           *(short *)(*(int *)(_DAT_800ac784 + 0x1b4) + 2) +
           *(short *)(*(int *)(_DAT_800ac784 + 0x1b4) + 6);
      if ((*(byte *)(_DAT_800ac784 + 0x90) & 0x40) == 0) {
        return;
      }
      *(undefined2 *)(_DAT_800ac784 + 0x1ee) = 1;
      *(undefined2 *)(_DAT_800ac784 + 0x1e8) = *(undefined2 *)(*(int *)(_DAT_800ac784 + 0x1b4) + 4);
      *(short *)(_DAT_800ac784 + 0x1ea) =
           **(short **)(_DAT_800ac784 + 0x1b4) + (*(short **)(_DAT_800ac784 + 0x1b4))[2];
      return;
    }
  }
  if (*(char *)(_DAT_800ac784 + 7) == '\0') {
    *(undefined1 *)(_DAT_800ac784 + 7) = 1;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
    uVar4 = func_0x8001af20();
    *(ushort *)(_DAT_800ac784 + 0x9c) = (uVar4 & 0x1f) * (ushort)*(byte *)(_DAT_800ac784 + 6) + 0x20
    ;
    if (*(byte *)(_DAT_800ac784 + 6) >> 1 == 0) {
      *(short *)(_DAT_800ac784 + 0x9c) = *(short *)(_DAT_800ac784 + 0x9c) + 0x50;
    }
  }
  else if (*(char *)(_DAT_800ac784 + 7) != '\x01') {
    return;
  }
  func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),*(undefined4 *)(_DAT_800ac784 + 0x16c),0,
                  0x200);
  if ((*(byte *)(_DAT_800ac784 + 0x95) == 0) || (uVar5 = 1, 8 < *(byte *)(_DAT_800ac784 + 0x95))) {
    sVar2 = *(short *)(_DAT_800ac784 + 0x166);
    uVar5 = 0;
  }
  else {
    sVar2 = *(short *)(_DAT_800ac784 + 0x166);
  }
  FUN_8011388c(uVar5,(int)sVar2);
  *(ushort *)(_DAT_800ac784 + 0x8c) = (ushort)*(byte *)(_DAT_800ac784 + 0x8c);
  sVar2 = *(short *)(_DAT_800ac784 + 0x9c);
  *(short *)(_DAT_800ac784 + 0x9c) = sVar2 + -1;
  if (sVar2 != 0) {
    return;
  }
  *(undefined2 *)(_DAT_800ac784 + 4) = 1;
  *(undefined2 *)(_DAT_800ac784 + 6) = 0;
  uVar3 = 0xd;
  if (1 < *(byte *)(_DAT_800ac784 + 9) - 3) {
    return;
  }
LAB_80111c48:
  *(undefined2 *)(_DAT_800ac784 + 6) = uVar3;
  return;
}


