/* _SsSetNrpnVabAttr12 @ 0x8007bd68  (Ghidra headless, raw MIPS overlay) */

void _SsSetNrpnVabAttr12(short param_1,short param_2,short param_3,VagAtr param_4,short param_5,
                        uchar param_6)

{
  undefined2 in_register_0000001e;
  uint uVar1;
  undefined4 local_resc;
  undefined1 auStack_30 [16];
  undefined2 local_20;
  
  local_resc._2_2_ = in_register_0000001e;
  local_resc._0_2_ = param_5;
  uVar1 = (uint)param_6;
  SsUtGetVagAtr(param_1,param_2,param_3,(VagAtr *)&local_resc);
  if ((uVar1 != 0) && (uVar1 < 0x40)) {
    local_20 = 0;
    DE_12_OBJ_88();
    return;
  }
  if (uVar1 - 0x40 < 0x40) {
    local_20 = 1;
  }
  _SsUtBuildADSR(auStack_30,&param_4.pbmin,&param_4.reserved1);
  SsUtSetVagAtr(param_1,param_2,param_3,(VagAtr *)&local_resc);
  return;
}


