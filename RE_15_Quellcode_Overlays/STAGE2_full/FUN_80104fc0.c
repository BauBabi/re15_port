/* FUN_80104fc0 @ 0x80104fc0  (Ghidra headless, raw MIPS overlay) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80104fc0(void)

{
  short sVar1;
  char cVar2;
  ushort uVar3;
  uint uVar4;
  undefined4 uVar5;
  
  switch(*(undefined1 *)(_DAT_800ac784 + 6)) {
  case 0:
    *(byte *)(_DAT_800ac784 + 0x93) = *(byte *)(_DAT_800ac784 + 0x93) | 1;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 0xb;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 6) = 1;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 0xf;
    uVar4 = func_0x8001af20();
    if ((uVar4 & 3) == 0) {
      uVar4 = func_0x8001af20();
      uVar5 = 8;
      if ((uVar4 & 1) != 0) {
        uVar5 = 5;
      }
      func_0x800453d0(uVar5);
    }
    *(undefined1 *)(_DAT_800ac784 + 0x1b8) = 1;
    *(undefined2 *)(_DAT_800ac784 + 0x1dc) = 0x80;
    *(byte *)(_DAT_800ac784 + 9) = *(byte *)(_DAT_800ac784 + 9) | 0x80;
  case 1:
    cVar2 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x170),
                            *(undefined4 *)(_DAT_800ac784 + 0x174),0,0x100);
    *(char *)(_DAT_800ac784 + 6) = *(char *)(_DAT_800ac784 + 6) + cVar2;
    FUN_80109230(0,0);
    break;
  case 2:
    *(byte *)(_DAT_800ac784 + 9) = *(byte *)(_DAT_800ac784 + 9) | 0x80;
    *(undefined1 *)(_DAT_800ac784 + 6) = 3;
    uVar4 = func_0x8001af20();
    *(short *)(_DAT_800ac784 + 0x9c) =
         (short)*(undefined4 *)(&DAT_80117c14 + (uVar4 & 0xf) * 4) * 0x1e;
    *(byte *)(_DAT_800ac784 + 0x93) = *(byte *)(_DAT_800ac784 + 0x93) & 0xfe;
  case 3:
    sVar1 = *(short *)(_DAT_800ac784 + 0x9c);
    *(short *)(_DAT_800ac784 + 0x9c) = sVar1 + -1;
    if (sVar1 == 0) {
      *(undefined1 *)(_DAT_800ac784 + 6) = 4;
    }
    break;
  case 4:
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 0x12;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 6) = 5;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 0xf;
    *(byte *)(_DAT_800ac784 + 0x93) = *(byte *)(_DAT_800ac784 + 0x93) | 1;
    *(undefined1 *)(_DAT_800ac784 + 0x1b8) = 0;
    uVar4 = func_0x8001af20();
    if ((uVar4 & 7) == 0) {
      uVar4 = func_0x8001af20();
      uVar5 = 8;
      if ((uVar4 & 1) != 0) {
        uVar5 = 5;
      }
      func_0x800453d0(uVar5);
    }
  case 5:
    cVar2 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x170),
                            *(undefined4 *)(_DAT_800ac784 + 0x174),0,0x100);
    *(char *)(_DAT_800ac784 + 6) = *(char *)(_DAT_800ac784 + 6) + cVar2;
    break;
  case 6:
    *(byte *)(_DAT_800ac784 + 9) = *(byte *)(_DAT_800ac784 + 9) & 0x7f;
    *(undefined4 *)(_DAT_800ac784 + 4) = 0x201;
    *(byte *)(_DAT_800ac784 + 0x93) = *(byte *)(_DAT_800ac784 + 0x93) & 0xfe;
    uVar3 = func_0x8001af20();
    *(ushort *)(_DAT_800ac784 + 0x1dc) = (uVar3 & 3) + 4;
  }
  return;
}


