/* FUN_8010f938 @ 0x8010f938  (Ghidra headless overlay RE) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010f938(void)

{
  short *psVar1;
  int iVar2;
  
  if (*(char *)(_DAT_800ac784 + 6) == '\0') {
    *(undefined1 *)(_DAT_800ac784 + 6) = 1;
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
  else if (*(char *)(_DAT_800ac784 + 6) != '\x01') {
    FUN_80110234();
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


