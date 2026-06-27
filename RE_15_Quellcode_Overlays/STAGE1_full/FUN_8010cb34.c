/* FUN_8010cb34 @ 0x8010cb34  (Ghidra headless, raw MIPS overlay) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010cb34(void)

{
  byte bVar1;
  ushort uVar2;
  short sVar3;
  int iVar4;
  
  bVar1 = *(byte *)(_DAT_800ac784 + 6);
  if (bVar1 != 1) {
    if (1 < bVar1) {
      if (bVar1 != 2) {
        if (bVar1 != 3) {
          return;
        }
        *(uint *)(*(int *)(_DAT_800ac784 + 0x188) + 0x158) =
             *(uint *)(*(int *)(_DAT_800ac784 + 0x188) + 0x158) & 0xfffffbff;
        *(uint *)(*(int *)(_DAT_800ac784 + 0x188) + 0x204) =
             *(uint *)(*(int *)(_DAT_800ac784 + 0x188) + 0x204) & 0xfffffbff;
        *(undefined1 *)(_DAT_800ac784 + 5) = 2;
        *(undefined1 *)(_DAT_800ac784 + 6) = 0;
        uVar2 = func_0x8001af20();
        *(ushort *)(_DAT_800ac784 + 0x1d0) = (uVar2 & 0x1f) + 0x1e;
        return;
      }
      iVar4 = *(int *)(_DAT_800ac784 + 0x188);
      *(short *)(iVar4 + 0x28e) = *(short *)(iVar4 + 0x28e) + *(char *)(iVar4 + 0x27a) * -2;
      iVar4 = *(int *)(_DAT_800ac784 + 0x188);
      *(short *)(iVar4 + 0x1e2) = *(short *)(iVar4 + 0x1e2) + *(char *)(iVar4 + 0x1ce) * -2;
      if (*(short *)(iVar4 + 400) < 0) {
        *(undefined1 *)(_DAT_800ac784 + 6) = 3;
      }
      *(short *)(iVar4 + 400) = *(short *)(iVar4 + 400) + -2;
      goto LAB_8010cda0;
    }
    if (bVar1 != 0) {
      return;
    }
    *(undefined1 *)(_DAT_800ac784 + 6) = 1;
    bVar1 = func_0x8001af20();
    *(byte *)(_DAT_800ac784 + 0x94) = bVar1 & 1;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
    *(undefined1 *)(_DAT_800ac784 + 0x9e) = 5;
    *(undefined2 *)(_DAT_800ac784 + 0x9c) = *(undefined2 *)(_DAT_800ac784 + 0x38);
    iVar4 = *(int *)(_DAT_800ac784 + 0x188);
    *(undefined2 *)(iVar4 + 0x1e0) = 0x1000;
    *(undefined2 *)(iVar4 + 0x1e2) = 0x1000;
    *(undefined2 *)(iVar4 + 0x1e4) = 0x1000;
    *(uint *)(iVar4 + 0x158) = *(uint *)(iVar4 + 0x158) | 0x400;
    uVar2 = func_0x8001af20();
    sVar3 = (uVar2 & 0xff) + 100;
    *(short *)(iVar4 + 400) = sVar3;
    *(short *)(iVar4 + 0x1da) = sVar3;
    bVar1 = func_0x8001af20();
    *(byte *)(iVar4 + 0x1ce) = (bVar1 & 7) + 5;
    iVar4 = *(int *)(_DAT_800ac784 + 0x188);
    *(undefined2 *)(iVar4 + 0x28c) = 0x1000;
    *(undefined2 *)(iVar4 + 0x28e) = 0x1000;
    *(undefined2 *)(iVar4 + 0x290) = 0x1000;
    *(uint *)(iVar4 + 0x204) = *(uint *)(iVar4 + 0x204) | 0x400;
    bVar1 = func_0x8001af20();
    *(byte *)(iVar4 + 0x27a) = bVar1 & 7;
  }
  *(int *)(_DAT_800ac784 + 0x38) =
       (int)*(short *)(_DAT_800ac784 + 0x9c) + (int)*(char *)(_DAT_800ac784 + 0x9e);
  *(char *)(_DAT_800ac784 + 0x9e) = -*(char *)(_DAT_800ac784 + 0x9e);
  iVar4 = *(int *)(_DAT_800ac784 + 0x188);
  *(short *)(iVar4 + 0x28e) = (short)*(char *)(iVar4 + 0x27a) + *(short *)(iVar4 + 0x28e);
  iVar4 = *(int *)(_DAT_800ac784 + 0x188);
  sVar3 = *(short *)(iVar4 + 0x1da);
  *(short *)(iVar4 + 0x1e2) = (short)*(char *)(iVar4 + 0x1ce) + *(short *)(iVar4 + 0x1e2);
  *(short *)(iVar4 + 0x1da) = sVar3 + -1;
  if (sVar3 == 0) {
    *(undefined1 *)(_DAT_800ac784 + 6) = 2;
  }
LAB_8010cda0:
  func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),*(undefined4 *)(_DAT_800ac784 + 0x16c),0,
                  0x200);
  return;
}


