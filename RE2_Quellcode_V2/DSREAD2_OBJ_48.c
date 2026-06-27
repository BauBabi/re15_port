/* DSREAD2_OBJ_48 @ 0x8008c794  (Ghidra headless, raw MIPS overlay) */

void DSREAD2_OBJ_48(void)

{
  _func_90 *func;
  DslCB func_00;
  int iVar1;
  u_char unaff_s0;
  DslLOC *unaff_s2;
  
  func = DsDataCallback(data_ready_callback);
  func_00 = DsReadyCallback(StCdInterrupt2);
  iVar1 = DsPacket(unaff_s0,unaff_s2,'\x1b',(DslCB)0x0,-1);
  if (iVar1 == 0) {
    DsDataCallback((func *)func);
    DsReadyCallback(func_00);
    DSREAD2_OBJ_C4();
    return;
  }
  return;
}


