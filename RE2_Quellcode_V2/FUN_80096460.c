/* FUN_80096460 @ 0x80096460  (Ghidra headless, raw MIPS overlay) */

void FUN_80096460(void)

{
  undefined *puVar1;
  
  DAT_800c39d4 = 0;
  FUN_800957a4();
  SysDeqIntRP(2,&DAT_800c40a8);
  SysEnqIntRP(2,&DAT_800c40a8);
  puVar1 = PTR_I_STAT_800c39fc;
  *(undefined4 *)PTR_I_STAT_800c39fc = 0xfffffffe;
  *(uint *)(puVar1 + 4) = *(uint *)(puVar1 + 4) | 1;
  ChangeClearRCnt(3,0);
  FUN_800957b4();
  (*DAT_800c39a0)(DAT_800c39d0);
  (*DAT_800c39a0)(DAT_800c39d0 + 0xf0);
  DAT_800c40bc = 0;
  DAT_800c40b8 = 0;
  DAT_800c39d4 = 1;
  return;
}


