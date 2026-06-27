/* S_N2P_OBJ_208 @ 0x80079d48  (Ghidra headless, raw MIPS overlay) */

uint S_N2P_OBJ_208(uint param_1,uint param_2)

{
  int iVar1;
  uint in_v1;
  int iVar2;
  uint uVar3;
  uint uVar4;
  uint uVar5;
  int iVar6;
  int iVar7;
  int iVar8;
  uint in_t8;
  int iVar9;
  
  iVar8 = 1 << (in_v1 & 0x1f);
  uVar4 = 0x1000;
  iVar6 = 0;
  iVar9 = iVar8 * 0x1000;
  do {
    uVar4 = uVar4 * 0x103b >> 0xc;
    iVar2 = 0;
    iVar7 = 0;
    uVar5 = iVar8 * uVar4 - iVar9 >> 5;
    uVar3 = uVar5;
    do {
      if (((uint)(iVar9 + iVar7) >> 0xc <= (param_1 & 0xffff)) &&
         (iVar1 = iVar6 * 0x20 + iVar2, (param_1 & 0xffff) < iVar9 + uVar3 >> 0xc))
      goto S_N2P_OBJ_2B0;
      uVar3 = uVar3 + uVar5;
      iVar2 = iVar2 + 1;
      iVar7 = iVar7 + uVar5;
    } while (iVar2 < 0x20);
    iVar6 = iVar6 + 1;
    iVar9 = iVar8 * uVar4;
  } while (iVar6 < 0x30);
  iVar1 = 0x600;
S_N2P_OBJ_2B0:
  iVar6 = iVar1;
  if (iVar1 < 0) {
    iVar6 = iVar1 + 0x7f;
  }
  return ((in_t8 & 0xffff) + (iVar6 >> 7) + (in_v1 - 0xc) * 0xc) * 0x100 |
         (param_2 & 0xffff) + iVar1 + (iVar6 >> 7) * -0x80;
}


