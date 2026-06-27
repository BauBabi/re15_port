/* FUN_80111120 @ 0x80111120  (Ghidra headless, raw MIPS overlay) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80111120(void)

{
  short *psVar1;
  int iVar2;
  char cVar3;
  
  iVar2 = _DAT_800ac784;
  switch(*(undefined1 *)(_DAT_800ac784 + 7)) {
  case 0:
    *(byte *)(_DAT_800ac784 + 0x93) = *(byte *)(_DAT_800ac784 + 0x93) | 2;
    *(undefined1 *)(_DAT_800ac784 + 4) = 2;
    *(undefined1 *)(_DAT_800ac784 + 5) = 0;
    *(undefined1 *)(_DAT_800ac784 + 6) = 0;
    *(undefined1 *)(_DAT_800ac784 + 7) = 0;
    func_0x8004ef90(0x800b1038,*(undefined1 *)(_DAT_800ac784 + 0x1c6));
    break;
  case 1:
    *(int *)(_DAT_800ac784 + 0x38) = (int)*(short *)(_DAT_800ac784 + 0x1ba);
    *(undefined1 *)(_DAT_800ac784 + 7) = 2;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 0xe;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x96) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
  case 2:
    cVar3 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                            *(undefined4 *)(_DAT_800ac784 + 0x16c),0,0x200);
    *(char *)(_DAT_800ac784 + 7) = *(char *)(_DAT_800ac784 + 7) + cVar3;
    if (*(char *)(_DAT_800ac784 + 7) == '\x03') {
      func_0x800453d0(7);
    }
    break;
  case 3:
    *(undefined4 *)(_DAT_800ac784 + 4) = 7;
    break;
  case 4:
    psVar1 = (short *)(_DAT_800ac784 + 0xbe);
    *(short *)(_DAT_800ac784 + 0xbc) = *(short *)(_DAT_800ac784 + 0xbc) + 8;
    *(short *)(iVar2 + 0xbe) = *psVar1 + 8;
    *(uint *)(iVar2 + 0xc4) = *(uint *)(iVar2 + 0xc4) & 0xff000000 | 0xffff38;
    *(uint *)(iVar2 + 0xec) = *(uint *)(iVar2 + 0xec) & 0xff000000 | 0xffff38;
    cVar3 = *(char *)(_DAT_800ac784 + 0x9e);
    *(char *)(_DAT_800ac784 + 0x9e) = cVar3 + -1;
    if (cVar3 == '\0') {
      *(undefined1 *)(_DAT_800ac784 + 7) = 5;
    }
  }
  return;
}


