/* DSSYS_2_OBJ_4FC @ 0x80089fb0  (Ghidra headless, raw MIPS overlay) */

void DSSYS_2_OBJ_4FC(void)

{
  code *in_v0;
  int iVar1;
  undefined1 unaff_s3;
  
  (*in_v0)();
  CQ_delete_command();
  if (DAT_800dcca8 != (code *)0x0) {
    (*DAT_800dcca8)(unaff_s3);
  }
  iVar1 = DS_system_status(0);
  if ((((iVar1 == 1) && (0 < DAT_800c3e80)) && (iVar1 = DS_system_status(0), iVar1 == 1)) &&
     ((&DAT_800c3db8)[DAT_800c3e7c * 6] != 0)) {
    DS_cw((&DAT_800c3dbc)[DAT_800c3e7c * 0x18],(&DAT_800c3dc4)[DAT_800c3e7c * 6]);
  }
  return;
}


