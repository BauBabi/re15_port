/* FUN_80021a48 @ 0x80021a48  (Ghidra headless, raw MIPS overlay) */

void FUN_80021a48(void)

{
  undefined4 *puVar1;
  int local_18;
  int local_14;
  int local_10;
  
  local_18 = (int)*(short *)(DAT_800dcbd0 + 0xd);
  local_14 = (int)*(short *)((int)DAT_800dcbd0 + 0x36);
  local_10 = (int)*(short *)(DAT_800dcbd0 + 0xe);
  FUN_8005ba28(0x3040001,&local_18);
  *(undefined1 *)((int)DAT_800dcbd0 + 0x21) = 0xd;
  *(undefined1 *)(DAT_800dcbd0 + 8) = 2;
  FUN_8001cbe8(0x30f2800,0,&DAT_8009db44,DAT_800dcbd0 + 0xd);
  puVar1 = DAT_800dcbd0;
  *(undefined1 *)((int)DAT_800dcbd0 + 9) = 0;
  *(undefined1 *)(puVar1 + 2) = 0;
  puVar1 = DAT_800dcbd0;
  *(undefined2 *)((int)DAT_800dcbd0 + 0xe) = 0;
  *(undefined2 *)(puVar1 + 3) = 0;
  *(undefined2 *)(puVar1 + 6) = 0xb001;
  *puVar1 = 0;
  return;
}


