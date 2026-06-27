void FUN_800537e4(void)

{
  SPRT *p;
  SPRT_8 *p_00;
  SPRT_16 *p_01;
  POLY_FT4 *p_02;
  int iVar1;
  int iVar2;
  undefined *puVar3;
  int iVar4;
  int iVar5;
  int iVar6;
  
  iVar6 = 0;
  iVar5 = 0;
  iVar4 = 0;
  do {
    iVar1 = 0;
    iVar2 = 0;
    do {
      p = (SPRT *)(&DAT_800c7934 + iVar4 + iVar2);
      SetSprt(p);
      iVar2 = iVar2 + 0x14;
      iVar1 = iVar1 + 1;
      p->r0 = 0x80;
      p->g0 = 0x80;
      p->b0 = 0x80;
    } while (iVar1 < 0x200);
    iVar1 = 0;
    puVar3 = &DAT_80091394;
    do {
      p_00 = (SPRT_8 *)(puVar3 + iVar5);
      SetSprt8(p_00);
      puVar3 = puVar3 + 0x10;
      iVar1 = iVar1 + 1;
      p_00->r0 = 0x80;
      p_00->g0 = 0x80;
      p_00->b0 = 0x80;
    } while (iVar1 < 0x80);
    iVar1 = 0;
    puVar3 = &DAT_80092394;
    do {
      p_01 = (SPRT_16 *)(puVar3 + iVar5);
      SetSprt16(p_01);
      puVar3 = puVar3 + 0x10;
      iVar1 = iVar1 + 1;
      p_01->r0 = 0x80;
      p_01->g0 = 0x80;
      p_01->b0 = 0x80;
    } while (iVar1 < 0x80);
    iVar1 = 0;
    iVar2 = 0;
    do {
      p_02 = (POLY_FT4 *)(&DAT_80093394 + iVar6 + iVar2);
      SetPolyFT4(p_02);
      iVar2 = iVar2 + 0x28;
      iVar1 = iVar1 + 1;
      p_02->r0 = 0x80;
      p_02->g0 = 0x80;
      p_02->b0 = 0x80;
    } while (iVar1 < 0x400);
    iVar6 = iVar6 + 0xa000;
    iVar4 = iVar4 + 0x2800;
    iVar5 = iVar5 + 0x800;
  } while (iVar4 < 0x5000);
  DAT_800a7394 = &UNK_8008ff94 + (uint)DAT_800aca34 * 0xa00;
  DAT_800a7398 = &DAT_80091394 + (uint)DAT_800aca34 * 0x800;
  DAT_800a739c = &DAT_80092394 + (uint)DAT_800aca34 * 0x800;
  DAT_800a73a0 = &DAT_80093394 + (uint)DAT_800aca34 * 0xa000;
  DAT_800a73a4 = DAT_800aca34 == 0;
  return;
}
