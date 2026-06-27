/* FUN_80111fcc @ 0x80111fcc  (Ghidra headless overlay RE) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80111fcc(undefined4 param_1,int param_2)

{
  char cVar1;
  uint in_v0;
  
  *(byte *)(param_2 + 0x1d5) =
       *(byte *)(param_2 + 0x1d4) +
       (char)(uint)((ulonglong)*(byte *)(param_2 + 0x1d4) * (ulonglong)(in_v0 | 0x8889) >> 0x25) *
       -0x3c;
  *(char *)(_DAT_800ac784 + 6) = *(char *)(_DAT_800ac784 + 6) + '\x01';
  cVar1 = *(char *)(_DAT_800ac784 + 0x1d5);
  *(char *)(_DAT_800ac784 + 0x1d5) = cVar1 + -1;
  if (cVar1 == '\0') {
    *(char *)(_DAT_800ac784 + 0x1d3) = *(char *)(_DAT_800ac784 + 0x1d3) + '\x01';
  }
  return;
}


