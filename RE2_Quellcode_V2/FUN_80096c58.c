/* FUN_80096c58 @ 0x80096c58  (Ghidra headless, raw MIPS overlay) */

undefined4 FUN_80096c58(void)

{
  ushort uVar1;
  undefined *puVar2;
  int iVar3;
  
  puVar2 = PTR_JOY_MCD_DATA_800c3a00;
  *(undefined4 *)PTR_I_STAT_800c39fc = 0xffffff7f;
  uVar1 = *(ushort *)(puVar2 + 4);
  while( true ) {
    if ((uVar1 & 0x80) == 0) {
      *(ushort *)(PTR_JOY_MCD_DATA_800c3a00 + 10) =
           *(ushort *)(PTR_JOY_MCD_DATA_800c3a00 + 10) | 0x10;
      return 1;
    }
    iVar3 = FUN_80098774();
    if (iVar3 != 0) break;
    uVar1 = *(ushort *)(PTR_JOY_MCD_DATA_800c3a00 + 4);
  }
  return 0;
}


