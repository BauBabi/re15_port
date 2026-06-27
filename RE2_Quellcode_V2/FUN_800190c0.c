/* FUN_800190c0 @ 0x800190c0  (Ghidra headless, raw MIPS overlay) */

void FUN_800190c0(int param_1)

{
  undefined4 uVar1;
  undefined4 uVar2;
  undefined4 uVar3;
  undefined4 uVar4;
  undefined4 uVar5;
  undefined4 uVar6;
  undefined4 uVar7;
  undefined4 uVar8;
  int iVar9;
  int iVar10;
  undefined4 uVar11;
  int iVar12;
  
  iVar12 = *(int *)(param_1 + 0x198);
  RotMatrix((SVECTOR *)(param_1 + 0x74),(MATRIX *)(param_1 + 0x24));
  iVar10 = iVar12 + 0x48;
  FUN_8002ce94((MATRIX *)(param_1 + 0x24),iVar12 + 0x18,iVar10);
  iVar9 = iVar12 + 0xf4;
  FUN_8002ce94(iVar10,iVar12 + 0xc4,iVar9);
  FUN_8002ce94(iVar9,iVar12 + 0x170,iVar12 + 0x1a0);
  FUN_8002ce94(iVar9,iVar12 + 0x374,iVar12 + 0x3a4);
  FUN_8002ce94(iVar10,iVar12 + 0x624,iVar12 + 0x654);
  FUN_8002ce94(iVar10,iVar12 + 0x828,iVar12 + 0x858);
  VSync(1);
  FUN_800197f4(param_1,iVar12,&DAT_8009a8e8);
  FUN_800197f4(param_1,iVar12,&DAT_8009a97c);
  FUN_800197f4(param_1,iVar12,&DAT_8009aa10);
  FUN_800197f4(param_1,iVar12,&DAT_8009aad8);
  FUN_800197f4(param_1,iVar12,&DAT_8009ab58);
  VSync(1);
  FUN_80019b3c(param_1);
  if (((DAT_800cfbd8 & 0x10000) != 0) &&
     ((DAT_800cfb7d == '\x02' ||
      (iVar9 = FUN_8002ea40(*(int *)(DAT_800ce324 + 0x24) + DAT_800d4820 * 0x20,DAT_800cfb7d,
                            param_1 + 0x38), iVar9 != 0)))) {
    FUN_8002e788(param_1);
    uVar11 = *(undefined4 *)(param_1 + 0x198);
    *(undefined4 *)(param_1 + 0x198) = *(undefined4 *)(param_1 + 0x1a4);
    FUN_800197f4(param_1,iVar12,&DAT_8009a8e8);
    FUN_800197f4(param_1,iVar12,&DAT_8009a97c);
    FUN_800197f4(param_1,iVar12,&DAT_8009aa10);
    FUN_800197f4(param_1,iVar12,&DAT_8009aad8);
    FUN_800197f4(param_1,iVar12,&DAT_8009ab58);
    uVar8 = DAT_800dcbc4;
    uVar7 = DAT_800dcbc0;
    uVar6 = DAT_800dcbbc;
    uVar5 = DAT_800dcbb8;
    uVar4 = DAT_800dcbb4;
    uVar3 = DAT_800dcbb0;
    uVar2 = DAT_800dcbac;
    uVar1 = DAT_800dcba8;
    DAT_800dcba8 = DAT_800e2a90;
    DAT_800dcbac = DAT_800e2a94;
    DAT_800dcbb0 = DAT_800e2a98;
    DAT_800dcbb4 = DAT_800e2a9c;
    DAT_800dcbb8 = DAT_800e2aa0;
    DAT_800dcbbc = DAT_800e2aa4;
    DAT_800dcbc0 = DAT_800e2aa8;
    DAT_800dcbc4 = DAT_800e2aac;
    FUN_80019b3c(param_1);
    DAT_800dcba8 = uVar1;
    DAT_800dcbac = uVar2;
    DAT_800dcbb0 = uVar3;
    DAT_800dcbb4 = uVar4;
    DAT_800dcbb8 = uVar5;
    DAT_800dcbbc = uVar6;
    DAT_800dcbc0 = uVar7;
    DAT_800dcbc4 = uVar8;
    *(undefined4 *)(param_1 + 0x198) = uVar11;
  }
  return;
}


