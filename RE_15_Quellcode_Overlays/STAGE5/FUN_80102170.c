/* FUN_80102170 @ 0x80102170  (Ghidra headless overlay RE) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80102170(short param_1,int param_2)

{
  char cVar1;
  short in_v1;
  
  *(short *)(param_2 + 0x9c) = (param_1 + -1) - in_v1;
  if (*(short *)(_DAT_800ac784 + 0x9c) < 0) {
    *(undefined1 *)(_DAT_800ac784 + 6) = 4;
    DAT_800aca5a = 4;
  }
  cVar1 = *(char *)(_DAT_800ac784 + 0x9e);
  *(char *)(_DAT_800ac784 + 0x9e) = cVar1 + -1;
  if ((cVar1 != '\0') && (-1 < _DAT_800acaee)) {
    return;
  }
  *(uint *)(_DAT_800ac784 + 4) = (*(byte *)(_DAT_800ac784 + 5) + 2) * 0x100 | 1;
  FUN_80102cf0();
  return;
}


