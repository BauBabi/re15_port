/* _SsSetNrpnVabAttr0 @ 0x8007b46c  (Ghidra headless, raw MIPS overlay) */

void _SsSetNrpnVabAttr0(short param_1,short param_2,short param_3,VagAtr param_4,short param_5,
                       uchar param_6)

{
  undefined2 in_register_0000001e;
  uchar local_resc;
  undefined3 uStackX_d;
  
  register0x0000001e = in_register_0000001e;
  _local_resc = param_5;
  SsUtGetVagAtr(param_1,param_2,param_3,(VagAtr *)&local_resc);
  local_resc = param_6;
  SsUtSetVagAtr(param_1,param_2,param_3,(VagAtr *)&local_resc);
  return;
}


