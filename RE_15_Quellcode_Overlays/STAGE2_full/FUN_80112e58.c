/* FUN_80112e58 @ 0x80112e58  (Ghidra headless, raw MIPS overlay) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80112e58(void)

{
  byte bVar1;
  int iVar2;
  char cVar3;
  ushort uVar4;
  short sVar5;
  
  bVar1 = *(byte *)(_DAT_800ac784 + 7);
  if (bVar1 != 1) {
    if (1 < bVar1) {
      if (bVar1 != 2) {
        return;
      }
      *(undefined1 *)(_DAT_800ac784 + 0x93) = 0;
      *(undefined1 *)(_DAT_800ac784 + 5) = 0;
      *(char *)(_DAT_800ac784 + 6) = (char)((*(ushort *)(_DAT_800ac784 + 0x1d0) & 1) << 2);
      *(undefined1 *)(_DAT_800ac784 + 7) = 0;
      *(undefined2 *)(_DAT_800ac784 + 0x1dc) = 0;
      return;
    }
    if (bVar1 != 0) {
      return;
    }
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
    *(undefined1 *)(_DAT_800ac784 + 7) = 1;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 6;
    uVar4 = func_0x8001af20();
    *(ushort *)(_DAT_800ac784 + 0x1e6) = (uVar4 & 1) * -0x658 + 0x32c;
    iVar2 = _DAT_800ac784;
    uVar4 = func_0x8001af20();
    sVar5 = *(short *)(_DAT_800ac784 + 0x1e6);
    if ((*(ushort *)(_DAT_800ac784 + 0x1d0) & 1) == 0) {
      sVar5 = sVar5 + (uVar4 & 0xff);
    }
    *(short *)(iVar2 + 0x1e6) = sVar5;
    uVar4 = func_0x8001af20();
    *(ushort *)(_DAT_800ac784 + 0x8c) = (uVar4 & 0x3f) + 200;
  }
  cVar3 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                          *(undefined4 *)(_DAT_800ac784 + 0x16c),0,0x200);
  *(char *)(_DAT_800ac784 + 7) = *(char *)(_DAT_800ac784 + 7) + cVar3;
  func_0x800245d8((int)*(short *)(_DAT_800ac784 + 0x1e6));
  return;
}


