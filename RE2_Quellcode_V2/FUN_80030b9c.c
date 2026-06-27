/* FUN_80030b9c @ 0x80030b9c  (Ghidra headless, raw MIPS overlay) */

undefined * FUN_80030b9c(uint param_1)

{
  ushort uVar1;
  int iVar2;
  uint uVar3;
  
  uVar3 = (uint)(byte)(&DAT_800a9e1d)[(param_1 & 0xff) * 8];
  if ((DAT_800cfb74 & 0x840) == 0x40) {
    if (uVar3 != 0) {
      iVar2 = FUN_80077360(&DAT_800d48ec,uVar3);
      if (iVar2 != 0) {
        return &UNK_8009e550 + *(ushort *)(&DAT_8009ecae + uVar3 * 2);
      }
    }
    return &UNK_8009e550 + *(ushort *)(&DAT_8009ebac + (param_1 & 0xff) * 2);
  }
  if (uVar3 != 0) {
    iVar2 = FUN_80077360(&DAT_800d48ec,uVar3);
    if (iVar2 != 0) {
      uVar1 = *(ushort *)(&DAT_8009e53a + uVar3 * 2);
      goto LAB_80030c78;
    }
  }
  uVar1 = *(ushort *)(&DAT_8009e438 + (param_1 & 0xff) * 2);
LAB_80030c78:
  return &UNK_8009df3c + uVar1;
}


