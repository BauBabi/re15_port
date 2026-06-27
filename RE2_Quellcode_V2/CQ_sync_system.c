/* CQ_sync_system @ 0x80089e58  (Ghidra headless, raw MIPS overlay) */

void CQ_sync_system(char param_1,undefined4 param_2)

{
  int iVar1;
  int iVar2;
  int iVar3;
  int *piVar4;
  
  iVar1 = DAT_800c3e7c;
  piVar4 = &DAT_800c3db8 + DAT_800c3e7c * 6;
  if (*piVar4 != 0) {
    DAT_800c3d88 = *piVar4;
    DAT_800c3d8c = param_1;
    rescpy(&DAT_800c3d8d);
    if (param_1 == '\x02') {
      iVar3 = DAT_800c3e7c + 1;
      iVar2 = iVar3;
      if (7 < iVar3) {
        iVar2 = 0;
      }
      if ((&DAT_800c3db8)[iVar2 * 6] == *piVar4) {
        DAT_800c3e7c = iVar3;
        if (7 < iVar3) {
          DAT_800c3e7c = 0;
          DSSYS_2_OBJ_50C();
          return;
        }
        goto code_r0x80089fc0;
      }
      CQ_add_result(*piVar4,2,param_2);
      if ((&DAT_800c3dc8)[iVar1 * 6] != 0) {
        DSSYS_2_OBJ_4FC(2);
        return;
      }
    }
    else {
      if (param_1 != '\x05') {
        DSSYS_2_OBJ_50C();
        return;
      }
      if ((0 < (int)(&DAT_800c3dcc)[iVar1 * 6]) || ((&DAT_800c3dcc)[iVar1 * 6] == -1)) {
        DAT_800c3e7c = DAT_800c3e78;
        if ((&DAT_800c3dcc)[iVar1 * 6] != -1) {
          (&DAT_800c3dcc)[iVar1 * 6] = (&DAT_800c3dcc)[iVar1 * 6] + -1;
          DSSYS_2_OBJ_50C();
          return;
        }
        goto code_r0x80089fc0;
      }
      CQ_add_result(*piVar4,5,param_2);
      if ((code *)(&DAT_800c3dc8)[iVar1 * 6] != (code *)0x0) {
        (*(code *)(&DAT_800c3dc8)[iVar1 * 6])(5,param_2);
      }
    }
    CQ_delete_command();
  }
code_r0x80089fc0:
  if (DAT_800dcca8 != (code *)0x0) {
    (*DAT_800dcca8)(param_1,param_2);
  }
  iVar1 = DS_system_status(0);
  if ((((iVar1 == 1) && (0 < DAT_800c3e80)) && (iVar1 = DS_system_status(0), iVar1 == 1)) &&
     ((&DAT_800c3db8)[DAT_800c3e7c * 6] != 0)) {
    DS_cw((&DAT_800c3dbc)[DAT_800c3e7c * 0x18],(&DAT_800c3dc4)[DAT_800c3e7c * 6]);
  }
  return;
}


