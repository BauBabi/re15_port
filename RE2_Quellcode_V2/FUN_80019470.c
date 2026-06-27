/* FUN_80019470 @ 0x80019470  (Ghidra headless, raw MIPS overlay) */

void FUN_80019470(int param_1)

{
  undefined4 uVar1;
  undefined4 uVar2;
  undefined4 uVar3;
  undefined4 uVar4;
  undefined4 uVar5;
  undefined4 uVar6;
  undefined4 uVar7;
  int iVar8;
  undefined4 uVar9;
  undefined4 uVar10;
  
  uVar9 = *(undefined4 *)(param_1 + 0x198);
  FUN_800197f4(param_1,uVar9,&DAT_8009adbc);
  FUN_80019cd0(param_1,uVar9);
  if ((DAT_800cfbd8 & 0x10000) != 0) {
    if ((DAT_800cfb7d == '\x02') ||
       (iVar8 = FUN_8002ea40(*(int *)(DAT_800ce324 + 0x24) + DAT_800d4820 * 0x20,DAT_800cfb7d,
                             param_1 + 0x38), iVar8 != 0)) {
      FUN_8002e788(param_1);
      uVar10 = *(undefined4 *)(param_1 + 0x198);
      *(undefined4 *)(param_1 + 0x198) = *(undefined4 *)(param_1 + 0x1a4);
      FUN_800197f4(param_1,uVar9,&DAT_8009adbc);
      uVar7 = DAT_800dcbc4;
      uVar6 = DAT_800dcbc0;
      uVar5 = DAT_800dcbbc;
      uVar4 = DAT_800dcbb8;
      uVar3 = DAT_800dcbb4;
      uVar2 = DAT_800dcbb0;
      uVar1 = DAT_800dcbac;
      uVar9 = DAT_800dcba8;
      DAT_800dcba8 = DAT_800e2a90;
      DAT_800dcbac = DAT_800e2a94;
      DAT_800dcbb0 = DAT_800e2a98;
      DAT_800dcbb4 = DAT_800e2a9c;
      DAT_800dcbb8 = DAT_800e2aa0;
      DAT_800dcbbc = DAT_800e2aa4;
      DAT_800dcbc0 = DAT_800e2aa8;
      DAT_800dcbc4 = DAT_800e2aac;
      FUN_80019cd0(param_1,*(undefined4 *)(param_1 + 0x198));
      DAT_800dcba8 = uVar9;
      DAT_800dcbac = uVar1;
      DAT_800dcbb0 = uVar2;
      DAT_800dcbb4 = uVar3;
      DAT_800dcbb8 = uVar4;
      DAT_800dcbbc = uVar5;
      DAT_800dcbc0 = uVar6;
      DAT_800dcbc4 = uVar7;
      *(undefined4 *)(param_1 + 0x198) = uVar10;
    }
  }
  return;
}


