/* FUN_80112d34 @ 0x80112d34  (Ghidra headless, raw MIPS overlay) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80112d34(void)

{
  char cVar1;
  
  if (*(char *)(_DAT_800ac784 + 6) == '\0') {
    *(byte *)(_DAT_800ac784 + 0x1d5) = *(byte *)(_DAT_800ac784 + 0x1d4) & 0x32;
    FUN_80115d94(6);
    func_0x800453d0(0);
    *(char *)(_DAT_800ac784 + 6) = *(char *)(_DAT_800ac784 + 6) + '\x01';
  }
  else if (*(char *)(_DAT_800ac784 + 6) != '\x01') {
    return;
  }
  cVar1 = *(char *)(_DAT_800ac784 + 0x1d5);
  *(char *)(_DAT_800ac784 + 0x1d5) = cVar1 + -1;
  if (cVar1 == '\0') {
    FUN_80115d74(9);
  }
  func_0x8001a8f8(&DAT_800aca88,0x32);
  cVar1 = FUN_80115dc8();
  *(ushort *)(_DAT_800ac784 + 0x1e4) = (*(byte *)(_DAT_800ac784 + 0x1d4) & 0x3f) * (short)cVar1;
  *(int *)(_DAT_800ac784 + 0x38) =
       (int)*(short *)(_DAT_800ac784 + 0x1e4) + *(int *)(_DAT_800ac784 + 0x38);
  func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),*(undefined4 *)(_DAT_800ac784 + 0x16c),0,
                  0x200);
  return;
}


