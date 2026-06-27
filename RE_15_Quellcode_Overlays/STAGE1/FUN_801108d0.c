/* FUN_801108d0 @ 0x801108d0  (Ghidra headless overlay RE) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801108d0(int param_1,uint param_2,uint param_3)

{
  char cVar1;
  undefined4 in_v0;
  uint in_v1;
  
  *(undefined4 *)(param_1 + 0xc4) = in_v0;
  *(uint *)(param_1 + 0xec) = in_v1 & param_2 | param_3;
  cVar1 = *(char *)(_DAT_800ac784 + 0x9e);
  *(char *)(_DAT_800ac784 + 0x9e) = cVar1 + -1;
  if (cVar1 == '\0') {
    *(undefined1 *)(_DAT_800ac784 + 7) = 4;
  }
  return;
}


