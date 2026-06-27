/* FUN_800532d8 @ 0x800532d8  (Ghidra headless, raw MIPS overlay) */

void FUN_800532d8(void)

{
  int iVar1;
  undefined4 uVar2;
  undefined4 uVar3;
  undefined4 uVar4;
  
  if ((DAT_800cfbd8 & 0x10000) != 0) {
    FUN_80041060(&DAT_800cfbf8);
    uVar4 = DAT_800d8cec;
    uVar3 = DAT_800d7858;
    uVar2 = DAT_800d7660;
    iVar1 = DAT_800cfd90;
    *(undefined4 *)(DAT_800cfd90 + 0x76c) = DAT_800d7840;
    *(undefined4 *)(iVar1 + 0x774) = uVar4;
    *(undefined4 *)(iVar1 + 0x770) = uVar2;
    *(undefined4 *)(iVar1 + 0x778) = uVar3;
    DAT_800ce32c = FUN_8002e2d8(&DAT_800cfbf8,DAT_800ce32c);
    FUN_800408c8(&DAT_800cfbf8);
    DAT_800ce32c = FUN_8002e4f8(&DAT_800cfbf8,DAT_800ce32c);
    FUN_8002e548(&DAT_800cfbf8);
  }
  return;
}


