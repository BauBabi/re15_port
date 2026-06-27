/* FUN_8003458c @ 0x8003458c  (Ghidra headless, raw MIPS overlay) */

void FUN_8003458c(void)

{
  int iVar1;
  int iVar2;
  uint uVar3;
  MATRIX *r0;
  int iVar4;
  
  uVar3 = (uint)DAT_800dcc1d;
  iVar1 = (uVar3 - DAT_800dcc1c) * 0x800;
  if (uVar3 == 0) {
    trap(0x1c00);
  }
  if ((uVar3 == 0xffffffff) && (iVar1 == -0x80000000)) {
    trap(0x1800);
  }
  iVar4 = iVar1 / (int)uVar3 + -0x400;
  iVar1 = *(int *)(DAT_800cfe14 + 0x198);
  iVar2 = rsin(iVar4);
  iVar2 = iVar2 + 0x1000;
  uVar3 = (int)DAT_800dcc04 - ((int)((uint)DAT_800dcc06 * iVar2) >> 0xd) & 0xfff;
  if ((DAT_800dcc1f & 0x40) == 0) {
    DAT_800dcc00 = DAT_800dcc00 - DAT_800dcc08;
  }
  else {
    if (DAT_800dcc1c == 0x50) {
      DAT_800dcc08 = DAT_800dcc08 / 2;
    }
    iVar4 = rcos(iVar4 * 3 + 0xc00);
    DAT_800dcc00 = (DAT_800dcc08 * (iVar4 + 0x1000) >> 0xc) + 0x9c4;
  }
  iVar4 = rcos(uVar3);
  DAT_800dcbe4 = DAT_800dcc20 + (DAT_800dcc00 * iVar4 >> 0xc);
  iVar4 = rsin(uVar3);
  DAT_800dcbec = DAT_800dcc24 + (DAT_800dcc00 * iVar4 >> 0xc);
  DAT_800dcbe8 = DAT_800dcc14 + (DAT_800dcc0c * iVar2 >> 0xd);
  DAT_800dcc20 = DAT_800dcc20 + (*(int *)(iVar1 + 0x5c) - DAT_800dcc20) / 0x1e;
  DAT_800dcc24 = DAT_800dcc24 + (*(int *)(iVar1 + 100) - DAT_800dcc24) / 0x1e;
  DAT_800dcbf0 = DAT_800dcbf0 + (*(int *)(iVar1 + 0x5c) - DAT_800dcbf0) / 0x1e;
  DAT_800dcbf4 = DAT_800dcc18 + (DAT_800dcc10 * iVar2 >> 0xd);
  DAT_800dcbf8 = DAT_800dcbf8 + (*(int *)(iVar1 + 100) - DAT_800dcbf8) / 0x1e;
  FUN_80076cb0(&DAT_800dcbe4,&DAT_800dcbf0);
  if (0xd0 < DAT_800dcbe0._2_2_ >> 7) {
    DAT_800dcbe0._2_2_ = DAT_800dcbe0._2_2_ - 0x80;
  }
  FUN_8008de24(DAT_800dcbe0._2_2_ >> 7);
  r0 = (MATRIX *)&DAT_800dcba8;
  gte_SetRotMatrix((MATRIX *)&DAT_800dcba8);
  gte_SetTransMatrix(r0);
  return;
}


