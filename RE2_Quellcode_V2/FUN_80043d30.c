/* FUN_80043d30 @ 0x80043d30  (Ghidra headless, raw MIPS overlay) */

void FUN_80043d30(int param_1)

{
  int iVar1;
  
  iVar1 = (*(byte *)(param_1 + 0x10e) - 2 & 0xff) * 6;
  FUN_8003947c((&DAT_800a6ed0)[iVar1],0);
  FUN_80039514((&DAT_800a6ed1)[iVar1],(&DAT_800a6ed2)[(*(ushort *)(param_1 + 0x10e) - 2) * 6],0);
  FUN_800395b8((&DAT_800a6ed5)[iVar1],(&DAT_800a6ed3)[iVar1],(&DAT_800a6ed4)[iVar1],
               (&DAT_800a6ed1)[iVar1]);
  return;
}


