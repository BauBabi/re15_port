/* DsRead2 @ 0x8008c74c  (Ghidra headless, raw MIPS overlay) */

int DsRead2(DslLOC *pos,int mode)

{
  int iVar1;
  _func_90 *func;
  DslCB func_00;
  
  if ((mode & 0x100U) == 0) {
    iVar1 = DsPacket((u_char)mode,pos,'\x1b',(DslCB)0x0,-1);
  }
  else {
    if ((mode & 0x20U) != 0) {
      DAT_800d4c6c = 0;
      iVar1 = DSREAD2_OBJ_48();
      return iVar1;
    }
    DAT_800d4c6c = 1;
    func = DsDataCallback(data_ready_callback);
    func_00 = DsReadyCallback(StCdInterrupt2);
    iVar1 = DsPacket((u_char)mode,pos,'\x1b',(DslCB)0x0,-1);
    if (iVar1 == 0) {
      DsDataCallback((func *)func);
      DsReadyCallback(func_00);
      iVar1 = DSREAD2_OBJ_C4();
      return iVar1;
    }
  }
  return iVar1;
}


