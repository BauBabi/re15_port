/* DsCommand @ 0x8008a380  (Ghidra headless, raw MIPS overlay) */

int DsCommand(u_char com,u_char *param,DslCB cbsync,int count)

{
  int iVar1;
  int *piVar2;
  int iVar3;
  
  if ((*(int *)(&DAT_800ad1ac + (uint)com * 4) != 0) && (param != (u_char *)0x0)) {
    if (7 < DAT_800c3e80) {
      iVar1 = DSSYS_2_OBJ_9F0();
      return iVar1;
    }
    iVar1 = DAT_800ad22c + 1;
    DAT_800ad22c = iVar1;
    piVar2 = (int *)CQ_last_queue();
    *piVar2 = iVar1;
    *(undefined1 *)(piVar2 + 1) = 2;
    parcpy((int)piVar2 + 5,param);
    piVar2[3] = (int)param;
    piVar2[4] = 0;
    piVar2[5] = 0;
    DAT_800c3e80 = DAT_800c3e80 + 1;
    iVar3 = DS_system_status(0);
    if ((iVar3 == 1) && ((&DAT_800c3db8)[DAT_800c3e7c * 6] == iVar1)) {
      CQ_execute();
    }
    if (iVar1 == 0) {
      return 0;
    }
  }
  iVar1 = 0;
  if (DAT_800c3e80 < 8) {
    iVar1 = DAT_800ad22c + 1;
    DAT_800ad22c = iVar1;
    piVar2 = (int *)CQ_last_queue();
    *piVar2 = iVar1;
    *(u_char *)(piVar2 + 1) = com;
    if (param != (u_char *)0x0) {
      parcpy((int)piVar2 + 5,param);
      piVar2[3] = (int)param;
      iVar1 = DSSYS_2_OBJ_A58();
      return iVar1;
    }
    piVar2[3] = 0;
    piVar2[4] = (int)cbsync;
    piVar2[5] = count;
    DAT_800c3e80 = DAT_800c3e80 + 1;
    iVar3 = DS_system_status(0);
    if ((iVar3 == 1) && ((&DAT_800c3db8)[DAT_800c3e7c * 6] == iVar1)) {
      CQ_execute();
    }
  }
  return iVar1;
}


