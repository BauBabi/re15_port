/* FUN_80119d08 @ 0x80119d08  (Ghidra headless, raw MIPS overlay) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80119d08(void)

{
  char cVar1;
  int iVar2;
  
  if (*(char *)((int)_DAT_800ac784 + 6) == '\0') {
    *(undefined1 *)((int)_DAT_800ac784 + 6) = 1;
    *(undefined1 *)(_DAT_800ac784 + 0x25) = 0xe;
    *(undefined1 *)((int)_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)((int)_DAT_800ac784 + 0x8f) = 7;
    *(undefined1 *)((int)_DAT_800ac784 + 0x9e) = 0x14;
  }
  else if (*(char *)((int)_DAT_800ac784 + 6) != '\x01') {
    return;
  }
  func_0x8001aac4(_DAT_800aca88,_DAT_800aca90,0x10);
  func_0x8001f314(_DAT_800ac784[0x21],_DAT_800ac784[0x5b],0,0x200);
  FUN_8011b5c4(1,1);
  iVar2 = (uint)(ushort)_DAT_800ac784[0x23] << 0x10;
  *(short *)(_DAT_800ac784 + 0x23) = (short)((iVar2 >> 0x10) - (iVar2 >> 0x1f) >> 1);
  *(short *)(_DAT_800ac784 + 0x23) = (short)_DAT_800ac784[0x23] + 0xa0;
  func_0x800245d8(0);
  if (((*_DAT_800ac784 & 0x10) != 0) &&
     (cVar1 = *(char *)((int)_DAT_800ac784 + 0x9e),
     *(char *)((int)_DAT_800ac784 + 0x9e) = cVar1 + -1, cVar1 == '\0')) {
    _DAT_800ac784[1] = 0x701;
    *(undefined2 *)(_DAT_800ac784 + 0x23) = 0;
  }
  return;
}


