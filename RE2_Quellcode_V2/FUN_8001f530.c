/* FUN_8001f530 @ 0x8001f530  (Ghidra headless, raw MIPS overlay) */

void FUN_8001f530(void)

{
  undefined1 *puVar1;
  int local_18;
  int local_14;
  int local_10;
  
  local_18 = (int)*(short *)(DAT_800dcbd0 + 0x34);
  local_14 = (int)*(short *)(DAT_800dcbd0 + 0x36);
  local_10 = (int)*(short *)(DAT_800dcbd0 + 0x38);
  FUN_8004fba0(&local_18,2,0x2000,0);
  puVar1 = DAT_800dcbd0;
  if (DAT_800dcbc8 != 0) {
    *DAT_800dcbd0 = 0;
    *(undefined2 *)(puVar1 + 0x18) = 0;
  }
  return;
}


