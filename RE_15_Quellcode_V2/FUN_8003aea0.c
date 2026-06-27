void FUN_8003aea0(void)

{
  int iVar1;
  int iVar2;
  int *piVar3;
  int iVar4;
  uint uVar5;
  
  iVar2 = *(int *)(DAT_800ac778 + 0x20);
  iVar4 = *(int *)(iVar2 + 4);
  uVar5 = 0;
  *(int *)(iVar2 + 4) = iVar2 + 0x18;
  piVar3 = (int *)(iVar2 + 8);
  do {
    iVar1 = *piVar3;
    *piVar3 = iVar2 + 0x18 + iVar4 * 0xc;
    piVar3 = piVar3 + 1;
    uVar5 = uVar5 + 1;
    iVar4 = iVar4 + iVar1;
  } while (uVar5 < 4);
  DAT_800b285c = FUN_8003bca8;
  DAT_800b2860 = &LAB_8003d00c;
  DAT_800b2864 = FUN_8003d6a8;
  DAT_800b2868 = &LAB_8003beb0;
  DAT_800b286c = &LAB_8003c734;
  DAT_800b2870 = &LAB_8003cb9c;
  DAT_800b2874 = &LAB_8003c2cc;
  DAT_800b2878 = &LAB_8003d7e8;
  DAT_800b287c = &LAB_8003d930;
  return;
}
