void FUN_80022da0(MATRIX *param_1,ushort *param_2,undefined2 *param_3)

{
  undefined4 uVar1;
  undefined4 uVar2;
  undefined4 uVar3;
  
  gte_SetRotMatrix(param_1);
  gte_ldsv_((uint)*param_2,(uint)param_2[3],(uint)param_2[6]);
  gte_rtir();
  uVar1 = gte_stIR1();
  uVar2 = gte_stIR2();
  uVar3 = gte_stIR3();
  *param_3 = (short)uVar1;
  param_3[3] = (short)uVar2;
  param_3[6] = (short)uVar3;
  gte_ldsv_((uint)param_2[1],(uint)param_2[4],(uint)param_2[7]);
  gte_rtir();
  uVar1 = gte_stIR1();
  uVar2 = gte_stIR2();
  uVar3 = gte_stIR3();
  param_3[1] = (short)uVar1;
  param_3[4] = (short)uVar2;
  param_3[7] = (short)uVar3;
  gte_ldsv_((uint)param_2[2],(uint)param_2[5],(uint)param_2[8]);
  gte_rtir();
  param_2 = param_2 + 10;
  uVar1 = gte_stIR1();
  uVar2 = gte_stIR2();
  uVar3 = gte_stIR3();
  param_3[2] = (short)uVar1;
  param_3[5] = (short)uVar2;
  param_3[8] = (short)uVar3;
  gte_SetTransMatrix(param_1);
  gte_ldVXY0(CONCAT22(param_2[2],*param_2));
  gte_ldVZ0(param_2 + 4);
  gte_rt();
  gte_stlvnl((VECTOR *)(param_3 + 10));
  return;
}
