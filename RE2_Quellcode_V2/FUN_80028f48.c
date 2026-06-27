/* FUN_80028f48 @ 0x80028f48  (Ghidra headless, raw MIPS overlay) */

void FUN_80028f48(SVECTOR *param_1,int param_2,MATRIX *param_3,int param_4)

{
  SVECTOR *pSVar1;
  undefined1 *puVar2;
  undefined1 auStack_20 [24];
  
  puVar2 = auStack_20;
  pSVar1 = (SVECTOR *)(0x1000 - param_4);
  gte_LoadAverageShort12(pSVar1,param_1,param_4,param_2,(SVECTOR *)param_3);
  gte_ldIR0(pSVar1);
  gte_ldsv(param_1 + 1);
  gte_gpf12();
  gte_ldIR0(param_4);
  gte_ldsv((SVECTOR *)(param_2 + 8));
  gte_gpl12();
  gte_stsv((SVECTOR *)(param_3->m[1] + 1));
  *(short *)(puVar2 + 0x10) = param_1->pad;
  *(short *)(puVar2 + 0x12) = param_1[1].pad;
  *(short *)(puVar2 + 0x14) = param_1[2].vx;
  gte_ldIR0(pSVar1);
  pSVar1 = (SVECTOR *)(puVar2 + 0x10);
  gte_ldsv(pSVar1);
  gte_gpf12();
  *(undefined2 *)(puVar2 + 0x10) = *(undefined2 *)(param_2 + 6);
  *(undefined2 *)(puVar2 + 0x12) = *(undefined2 *)(param_2 + 0xe);
  *(undefined2 *)(puVar2 + 0x14) = *(undefined2 *)(param_2 + 0x10);
  gte_ldIR0(param_4);
  gte_ldsv(pSVar1);
  gte_gpl12();
  gte_stsv(pSVar1);
  param_3->m[1][0] = *(short *)(puVar2 + 0x10);
  param_3->m[2][1] = *(short *)(puVar2 + 0x12);
  param_3->m[2][2] = *(short *)(puVar2 + 0x14);
  MatrixNormal_0(param_3,param_3);
  return;
}


