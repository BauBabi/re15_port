/* FUN_8010e510 @ 0x8010e510  (Ghidra headless overlay RE) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010e510(undefined4 param_1,undefined4 param_2,uint param_3)

{
  uint *puVar1;
  char cVar2;
  int iVar3;
  uint *in_v1;
  
  *in_v1 = *in_v1 | 0x40;
  iVar3 = _DAT_800ac784;
  puVar1 = (uint *)(_DAT_800ac784 + 0xec);
  *(uint *)(_DAT_800ac784 + 0xc4) = *(uint *)(_DAT_800ac784 + 0xc4) & 0xff000000 | param_3 | 0xff38;
  *(uint *)(iVar3 + 0xec) = *puVar1 & 0xff000000 | param_3 | 0xff38;
  *(short *)(iVar3 + 0xbc) = *(short *)(iVar3 + 0xbc) + 8;
  *(short *)(iVar3 + 0xbe) = *(short *)(iVar3 + 0xbe) + 8;
  cVar2 = *(char *)(_DAT_800ac784 + 0x9e);
  *(char *)(_DAT_800ac784 + 0x9e) = cVar2 + -1;
  if (cVar2 == '\0') {
    *(undefined1 *)(_DAT_800ac784 + 7) = 2;
  }
  return;
}


