/* FUN_8002ce94 @ 0x8002ce94  (Ghidra headless, raw MIPS overlay) */

void FUN_8002ce94(MATRIX *param_1,MATRIX *param_2,MATRIX *param_3)

{
  MATRIX *pMVar1;
  MATRIX *r0;
  MATRIX *r0_00;
  VECTOR *r0_01;
  
  gte_SetRotMatrix(param_1);
  gte_SetTransMatrix(param_1);
  gte_ldclmv(param_2);
  gte_rtir();
  pMVar1 = (MATRIX *)(param_2->m[0] + 1);
  r0 = (MATRIX *)(param_3->m[0] + 1);
  gte_stclmv(param_3);
  gte_ldclmv(pMVar1);
  gte_rtir();
  r0_00 = (MATRIX *)(param_2->m[0] + 2);
  pMVar1 = (MATRIX *)(param_3->m[0] + 2);
  gte_stclmv(r0);
  gte_ldclmv(r0_00);
  gte_rtir();
  r0_01 = (VECTOR *)param_2->t;
  gte_stclmv(pMVar1);
  gte_ldlv0(r0_01);
  gte_rt();
  gte_stlvnl((VECTOR *)param_3->t);
  return;
}


