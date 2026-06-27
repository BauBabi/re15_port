undefined2 * ApplyMatrixSV(undefined4 *param_1,SVECTOR *param_2,undefined2 *param_3)

{
  undefined4 uVar1;
  undefined4 uVar2;
  undefined4 uVar3;
  
  gte_ldR11R12(*param_1);
  gte_ldR13R21(param_1[1]);
  gte_ldR22R23(param_1[2]);
  gte_ldR31R32(param_1[3]);
  gte_ldR33(param_1[4]);
  gte_ldv0(param_2);
  gte_rtv0_b();
  uVar1 = gte_stIR1();
  uVar2 = gte_stIR2();
  uVar3 = gte_stIR3();
  *param_3 = (short)uVar1;
  param_3[1] = (short)uVar2;
  param_3[2] = (short)uVar3;
  return param_3;
}
