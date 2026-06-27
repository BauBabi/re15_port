/* FUN_8006a2b4 @ 0x8006a2b4  (Ghidra headless, raw MIPS overlay) */

void FUN_8006a2b4(void)

{
  byte bVar1;
  int iVar2;
  uint uVar3;
  
  bVar1 = (&DAT_800a9e1c)[(uint)(byte)(&DAT_800d4a3c)[(uint)DAT_800d5bf8 * 4] * 8];
  iVar2 = FUN_800696cc(*(&PTR_DAT_800a9e20)[(uint)(byte)(&DAT_800d4a3c)[(uint)DAT_800d5bf8 * 4] * 2]
                      );
  iVar2 = iVar2 * 4;
  uVar3 = (uint)(byte)(&DAT_800d4a3d)[iVar2];
  if (bVar1 < uVar3) {
    (&DAT_800d4a3d)[iVar2] = (&DAT_800d4a3d)[iVar2] - bVar1;
    uVar3 = (uint)bVar1;
  }
  else {
    (&DAT_800d4a3c)[iVar2] = 0;
    FUN_80069714();
  }
  FUN_800694b8((uint)DAT_800d5bf8,(byte)(&DAT_800d4a3d)[(uint)DAT_800d5bf8 * 4] + uVar3);
  return;
}


