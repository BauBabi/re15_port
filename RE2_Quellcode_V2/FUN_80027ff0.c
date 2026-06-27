/* FUN_80027ff0 @ 0x80027ff0  (Ghidra headless, raw MIPS overlay) */

void FUN_80027ff0(undefined4 param_1,undefined4 *param_2,uint param_3,MATRIX *param_4)

{
  uint uVar1;
  VECTOR *pVVar2;
  MATRIX *pMVar3;
  MATRIX *pMVar4;
  MATRIX *pMVar5;
  undefined1 *puVar6;
  undefined1 *puVar7;
  undefined1 auStack_50 [64];
  
  puVar6 = auStack_50;
  pMVar3 = (MATRIX *)param_2[0x1d];
  pMVar4 = (MATRIX *)(param_2 + 6);
  if ((*(uint *)param_2[0x25] & 0x21) == 0x20) {
    *param_2 = 0x20;
  }
  else {
    uVar1 = param_3 & 0x20;
    puVar7 = auStack_50;
    if ((param_3 & 0x40) == 0) {
      gte_SetRotMatrix(pMVar3);
      gte_SetTransMatrix(pMVar3);
      gte_ldclmv(pMVar4);
      gte_rtir();
      pMVar3 = (MATRIX *)((int)param_2 + 0x1a);
      pMVar5 = (MATRIX *)(param_4->m[0] + 1);
      gte_stclmv(param_4);
      gte_ldclmv(pMVar3);
      gte_rtir();
      pMVar3 = (MATRIX *)(param_2 + 7);
      pMVar4 = (MATRIX *)(param_4->m[0] + 2);
      gte_stclmv(pMVar5);
      gte_ldclmv(pMVar3);
      gte_rtir();
      pVVar2 = (VECTOR *)(param_2 + 0xb);
      gte_stclmv(pMVar4);
      gte_ldlv0(pVVar2);
      gte_rt();
      gte_stlvnl((VECTOR *)param_4->t);
      uVar1 = param_3 & 0x20;
      puVar7 = puVar6;
    }
    if (uVar1 != 0) {
      FUN_80028ad8(param_2,param_4);
    }
    pMVar3 = (MATRIX *)&DAT_800dcba8;
    gte_SetRotMatrix((MATRIX *)&DAT_800dcba8);
    gte_SetTransMatrix(pMVar3);
    gte_ldclmv(param_4);
    gte_rtir();
    pMVar3 = (MATRIX *)(param_4->m[0] + 1);
    pMVar5 = (MATRIX *)(puVar7 + 0x12);
    gte_stclmv((MATRIX *)(puVar7 + 0x10));
    gte_ldclmv(pMVar3);
    gte_rtir();
    pMVar3 = (MATRIX *)(param_4->m[0] + 2);
    pMVar4 = (MATRIX *)(puVar7 + 0x14);
    gte_stclmv(pMVar5);
    gte_ldclmv(pMVar3);
    gte_rtir();
    pVVar2 = (VECTOR *)param_4->t;
    gte_stclmv(pMVar4);
    gte_ldlv0(pVVar2);
    gte_rt();
    gte_stlvnl((VECTOR *)(puVar7 + 0x24));
    if ((param_3 & 8) == 0) {
      if ((param_3 & 0x10) != 0) {
        FUN_80028ea4(param_2);
      }
    }
    else {
      FUN_80028dac(param_2);
    }
  }
  return;
}


