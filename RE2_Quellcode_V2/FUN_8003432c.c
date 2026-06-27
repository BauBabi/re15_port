/* FUN_8003432c @ 0x8003432c  (Ghidra headless, raw MIPS overlay) */

void FUN_8003432c(void)

{
  int iVar1;
  int iVar2;
  undefined4 *puVar3;
  long lVar4;
  int iVar5;
  uint uVar6;
  
  puVar3 = (undefined4 *)(DAT_800d4820 * 0x20 + *(int *)(DAT_800ce324 + 0x24));
  iVar5 = *(int *)(DAT_800cfe14 + 0x198);
  DAT_800dcbe0 = *puVar3;
  DAT_800dcbe4 = puVar3[1];
  DAT_800dcbe8 = puVar3[2];
  DAT_800dcbec = puVar3[3];
  DAT_800dcbf0 = puVar3[4];
  DAT_800dcbf4 = puVar3[5];
  DAT_800dcbf8 = puVar3[6];
  DAT_800dcbfc = puVar3[7];
  lVar4 = ratan2(DAT_800dcbec - DAT_800dcbf8,DAT_800dcbe4 - DAT_800dcbf0);
  iVar2 = DAT_800dcbf4;
  iVar1 = DAT_800dcbe8;
  DAT_800dcc04 = (ushort)lVar4 & 0xfff;
  DAT_800dcc06 = DAT_800dcc04 + *(short *)(DAT_800cfe14 + 0x76) + -300 & 0xfff;
  if ((DAT_800dcc1f & 0x40) != 0) {
    DAT_800dcc06 = DAT_800dcc06 + 0x1000;
  }
  DAT_800dcbe8 = 0;
  DAT_800dcbf4 = 0;
  DAT_800dcc20 = DAT_800dcbf0;
  DAT_800dcc24 = DAT_800dcbf8;
  DAT_800dcc00 = SquareRoot0((DAT_800dcbe4 - DAT_800dcbf0) * (DAT_800dcbe4 - DAT_800dcbf0) +
                             (DAT_800dcbec - DAT_800dcbf8) * (DAT_800dcbec - DAT_800dcbf8));
  DAT_800dcbe8 = iVar1;
  DAT_800dcbf4 = iVar2;
  if ((DAT_800dcc1f & 0x40) == 0) {
    uVar6 = (uint)DAT_800dcc1d;
    DAT_800dcc08 = (DAT_800dcc00 + -0x9c4) / (int)uVar6;
    if (uVar6 == 0) {
      trap(0x1c00);
    }
    if ((uVar6 == 0xffffffff) && (DAT_800dcc00 + -0x9c4 == -0x80000000)) {
      trap(0x1800);
    }
  }
  else {
    DAT_800dcc08 = DAT_800dcc00 + -0x9c4 >> 1;
  }
  DAT_800dcc14 = iVar1;
  DAT_800dcc18 = iVar2;
  DAT_800dcc0c = (*(int *)(iVar5 + 0x60) + 400) - iVar1;
  DAT_800dcc10 = (*(int *)(iVar5 + 0x60) + -200) - iVar2;
  return;
}


