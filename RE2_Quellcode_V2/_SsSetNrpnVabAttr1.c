/* _SsSetNrpnVabAttr1 @ 0x8007b4fc  (Ghidra headless, raw MIPS overlay) */

void _SsSetNrpnVabAttr1(short param_1,short param_2,short param_3,VagAtr param_4,short param_5,
                       uchar param_6)

{
  undefined2 in_register_0000001e;
  uchar local_resc;
  uchar uStackX_d;
  undefined2 uStackX_e;
  
  uStackX_e = in_register_0000001e;
  _local_resc = param_5;
  SsUtGetVagAtr(param_1,param_2,param_3,(VagAtr *)&stack0x0000000c);
  uStackX_d = param_6;
  SsUtSetVagAtr(param_1,param_2,param_3,(VagAtr *)&stack0x0000000c);
  if (param_6 == '\0') {
    FUN_80081328();
    DE_1_OBJ_98();
    return;
  }
  if (param_6 == '\x04') {
    FUN_80081348();
  }
  return;
}


