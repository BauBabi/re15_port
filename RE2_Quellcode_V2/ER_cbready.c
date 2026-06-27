/* ER_cbready @ 0x8008c938  (Ghidra headless, raw MIPS overlay) */

void ER_cbready(char param_1,byte *param_2)

{
  DslLOC *p;
  uint uVar1;
  _func_90 *func;
  int iVar2;
  DslLOC aDStack_28 [4];
  
  if (DAT_800ad32c == -1) {
    p = (DslLOC *)DS_lastpos();
    DAT_800ad32c = DsPosToInt(p);
  }
  if (param_1 == '\x01') {
    uVar1 = DS_lastmode();
    if ((uVar1 & 0x20) == 0) {
      if ((DAT_800ad334 != (code *)0x0) && (DAT_800ad330 < DAT_800ad32c)) {
        (*DAT_800ad334)(1,param_2,aDStack_28);
        DAT_800ad330 = DAT_800ad32c;
      }
code_r0x8008ca6c:
      DAT_800ad32c = DAT_800ad32c + 1;
      DSREADY_OBJ_2D4();
      return;
    }
    func = DsDataCallback((func *)0x0);
    DsGetSector(aDStack_28,3);
    DsDataCallback((func *)func);
    iVar2 = DsPosToInt(aDStack_28);
    if (iVar2 == DAT_800ad32c) {
      if ((DAT_800ad334 != (code *)0x0) && (DAT_800ad330 < iVar2)) {
        (*DAT_800ad334)(1,param_2,aDStack_28);
        DAT_800ad330 = iVar2;
        DSREADY_OBJ_220();
        return;
      }
      goto code_r0x8008ca6c;
    }
  }
  else {
    if (param_1 == '\x04') {
      if (DAT_800ad334 != (code *)0x0) {
        (*DAT_800ad334)(4,param_2,aDStack_28);
        DSREADY_OBJ_2D4();
        return;
      }
      goto code_r0x8008cb20;
    }
    if ((*param_2 & 0x10) != 0) {
      if (DAT_800ad334 == (code *)0x0) {
        return;
      }
      (*DAT_800ad334)(5,param_2,aDStack_28);
      DSREADY_OBJ_358();
      return;
    }
    iVar2 = DsQueueLen();
    if ((iVar2 != 0) || ((*param_2 & 0xa0) != 0)) goto code_r0x8008cb20;
  }
  DAT_800ad338 = 1;
code_r0x8008cb20:
  if (DAT_800ad338 == 1) {
    if ((DAT_800ad33c < 1) && (DAT_800ad33c != -1)) {
      if (DAT_800ad334 != (code *)0x0) {
        (*DAT_800ad334)(5,param_2,aDStack_28);
      }
    }
    else {
      ER_retry();
      if (0 < DAT_800ad33c) {
        DAT_800ad33c = DAT_800ad33c + -1;
        DSREADY_OBJ_350();
        return;
      }
    }
    DAT_800ad338 = 0;
  }
  return;
}


