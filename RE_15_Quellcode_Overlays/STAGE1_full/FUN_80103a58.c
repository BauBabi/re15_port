/* FUN_80103a58 @ 0x80103a58  (Ghidra headless, raw MIPS overlay) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80103a58(void)

{
  byte bVar1;
  short sVar2;
  char cVar3;
  
  bVar1 = *(byte *)(_DAT_800ac784 + 6);
  if (bVar1 == 1) {
    sVar2 = *(short *)(_DAT_800ac784 + 0x9c);
    *(short *)(_DAT_800ac784 + 0x9c) = sVar2 + -1;
    if (sVar2 == 0) {
      *(undefined1 *)(_DAT_800ac784 + 6) = 2;
      *(undefined1 *)(_DAT_800ac784 + 0x8f) = 0xf;
    }
  }
  else if (bVar1 < 2) {
    if (bVar1 == 0) {
      *(byte *)(_DAT_800ac784 + 0x93) = *(byte *)(_DAT_800ac784 + 0x93) | 1;
      *(undefined1 *)(_DAT_800ac784 + 0x1b8) = 1;
    }
  }
  else if (bVar1 == 2) {
    cVar3 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x170),
                            *(undefined4 *)(_DAT_800ac784 + 0x174),0,0x100);
    *(char *)(_DAT_800ac784 + 6) = *(char *)(_DAT_800ac784 + 6) + cVar3;
  }
  else if (bVar1 == 3) {
    *(undefined1 *)(_DAT_800ac784 + 9) = 0;
    *(undefined4 *)(_DAT_800ac784 + 4) = 0x201;
    *(byte *)(_DAT_800ac784 + 0x93) = *(byte *)(_DAT_800ac784 + 0x93) & 0xfe;
    *(undefined1 *)(_DAT_800ac784 + 0x1b8) = 0;
  }
  return;
}


