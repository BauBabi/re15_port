/* FUN_8001f128 @ 0x8001f128  (Ghidra headless, raw MIPS overlay) */

void FUN_8001f128(void)

{
  undefined1 *puVar1;
  undefined1 *puVar2;
  undefined4 uVar3;
  int local_18;
  int local_14;
  int local_10;
  
  local_18 = (int)*(short *)(DAT_800dcbd0 + 0x34);
  local_14 = (int)*(short *)(DAT_800dcbd0 + 0x36);
  local_10 = (int)*(short *)(DAT_800dcbd0 + 0x38);
  uVar3 = FUN_8004fba0(&local_18,2,0x2000,0);
  puVar2 = DAT_800dcbd0;
  puVar1 = DAT_800dcbd0 + 0xb;
  *(undefined4 *)(DAT_800dcbd0 + 0x14) = uVar3;
  *puVar2 = *puVar1;
  DAT_800dcbd0[1] = 5;
  return;
}


