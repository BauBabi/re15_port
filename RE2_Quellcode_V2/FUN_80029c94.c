/* FUN_80029c94 @ 0x80029c94  (Ghidra headless, raw MIPS overlay) */

void FUN_80029c94(void)

{
  undefined4 uVar1;
  undefined4 uVar2;
  undefined4 uVar3;
  int iVar4;
  undefined4 *puVar5;
  int iVar6;
  uint uVar7;
  undefined4 uVar8;
  undefined4 uVar9;
  undefined4 uVar10;
  undefined4 uVar11;
  
  uVar2 = DAT_800d8cec;
  uVar1 = DAT_800d7858;
  uVar3 = DAT_800d7660;
  iVar6 = DAT_800cfd90;
  uVar7 = (uint)DAT_800cfcff;
  if (DAT_800cfc0c != 0) {
    uVar8 = *(undefined4 *)(DAT_800cfd90 + 0x76c);
    uVar9 = *(undefined4 *)(DAT_800cfd90 + 0x774);
    uVar10 = *(undefined4 *)(DAT_800cfd90 + 0x770);
    uVar11 = *(undefined4 *)(DAT_800cfd90 + 0x778);
    *(undefined4 *)(DAT_800cfd90 + 0x76c) = DAT_800d7840;
    *(undefined4 *)(iVar6 + 0x774) = uVar2;
    *(undefined4 *)(iVar6 + 0x770) = uVar3;
    *(undefined4 *)(iVar6 + 0x778) = uVar1;
    if (uVar7 != 0) {
      puVar5 = (undefined4 *)(DAT_800cfd90 + 0x14);
      iVar6 = DAT_800cfd90;
      do {
        uVar7 = uVar7 - 1;
        iVar4 = iVar6 + 4;
        iVar6 = iVar6 + 0xac;
        FUN_8002cbc4(iVar4,puVar5[-2],1);
        uVar3 = *puVar5;
        puVar5 = puVar5 + 0x2b;
        FUN_8002cd24(iVar4,uVar3,1);
      } while (uVar7 != 0);
    }
    iVar6 = DAT_800cfd90;
    *(undefined4 *)(DAT_800cfd90 + 0x76c) = uVar8;
    *(undefined4 *)(iVar6 + 0x774) = uVar9;
    *(undefined4 *)(iVar6 + 0x770) = uVar10;
    *(undefined4 *)(iVar6 + 0x778) = uVar11;
    if (((DAT_800cfc00 == '\0') || (DAT_800cfc00 == '\x02')) &&
       (iVar6 = FUN_80077360(&DAT_800d4854,0x3e), iVar6 != 0)) {
      FUN_80017054(DAT_800cfd90 + 4,DAT_800cfcfc + 2,0,DAT_800cfcfd + 0x1e2);
    }
  }
  return;
}


