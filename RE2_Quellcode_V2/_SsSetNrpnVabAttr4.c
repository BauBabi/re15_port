/* _SsSetNrpnVabAttr4 @ 0x8007b6d8  (Ghidra headless, raw MIPS overlay) */

void _SsSetNrpnVabAttr4(short param_1,short param_2,short param_3,VagAtr param_4,short param_5,
                       uchar param_6)

{
  undefined2 in_register_0000001e;
  undefined4 local_resc;
  ushort local_30 [5];
  undefined2 local_26;
  
  local_resc._2_2_ = in_register_0000001e;
  local_resc._0_2_ = param_5;
  SsUtGetVagAtr(param_1,param_2,param_3,(VagAtr *)&local_resc);
  _SsUtResolveADSR(param_4._12_2_,param_4._14_2_,local_30);
  local_26 = 0;
  local_30[0] = (ushort)param_6;
  _SsUtBuildADSR(local_30,&param_4.pbmin,&param_4.reserved1);
  SsUtSetVagAtr(param_1,param_2,param_3,(VagAtr *)&local_resc);
  return;
}


