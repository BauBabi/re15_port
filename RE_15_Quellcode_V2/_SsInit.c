void _SsInit(void)

{
  undefined2 uVar1;
  word wVar2;
  undefined4 *puVar3;
  undefined2 *puVar4;
  word *pwVar5;
  undefined *puVar6;
  int iVar7;
  int iVar8;
  dword *pdVar9;
  word *pwVar10;
  
  pdVar9 = &VOICE_00_LEFT_RIGHT;
  iVar7 = 0;
  do {
    iVar8 = 0;
    puVar4 = &DAT_800776a4;
    do {
      uVar1 = *puVar4;
      puVar4 = puVar4 + 1;
      iVar8 = iVar8 + 1;
      *(undefined2 *)pdVar9 = uVar1;
      pdVar9 = (dword *)((int)pdVar9 + 2);
    } while (iVar8 < 8);
    iVar7 = iVar7 + 1;
  } while (iVar7 < 0x18);
  pwVar10 = &SPU_MAIN_VOL_L;
  iVar7 = 0;
  pwVar5 = &DAT_800776b4;
  do {
    wVar2 = *pwVar5;
    pwVar5 = pwVar5 + 1;
    iVar7 = iVar7 + 1;
    *pwVar10 = wVar2;
    pwVar10 = pwVar10 + 1;
  } while (iVar7 < 0x10);
  SpuVmInit(0x18);
  iVar7 = 0;
  puVar6 = &UNK_800bb580;
  do {
    iVar8 = 0xf;
    puVar3 = (undefined4 *)(puVar6 + 0x3c);
    do {
      *puVar3 = 0;
      iVar8 = iVar8 + -1;
      puVar3 = puVar3 + -1;
    } while (-1 < iVar8);
    iVar7 = iVar7 + 1;
    puVar6 = puVar6 + 0x40;
  } while (iVar7 < 0x20);
  DAT_800bb4bc = 0x3c;
  DAT_800b52e4 = 0;
  DAT_800b283c = 0;
  return;
}
