/* _SsSetNrpnVabAttr11 @ 0x8007bcc0  (Ghidra headless, raw MIPS overlay) */

void _SsSetNrpnVabAttr11(short param_1,short param_2,short param_3,VagAtr param_4,short param_5,
                        uchar param_6)

{
  undefined2 in_register_0000001e;
  undefined4 local_resc;
  undefined1 auStack_30 [8];
  ushort local_28;
  undefined2 local_22;
  
  local_resc._2_2_ = in_register_0000001e;
  local_resc._0_2_ = param_5;
  SsUtGetVagAtr(param_1,param_2,param_3,(VagAtr *)&local_resc);
  local_22 = 1;
  local_28 = (ushort)param_6;
  _SsUtBuildADSR(auStack_30,&param_4.pbmin,&param_4.reserved1);
  SsUtSetVagAtr(param_1,param_2,param_3,(VagAtr *)&local_resc);
  return;
}


