/* CQ_vsync_system @ 0x8008acc4  (Ghidra headless, raw MIPS overlay) */

void CQ_vsync_system(void)

{
  int iVar1;
  
  iVar1 = DS_system_status(0);
  if ((((iVar1 == 1) && (0 < DAT_800c3e80)) && (iVar1 = DS_system_status(0), iVar1 == 1)) &&
     ((&DAT_800c3db8)[DAT_800c3e7c * 6] != 0)) {
    DS_cw((&DAT_800c3dbc)[DAT_800c3e7c * 0x18],(&DAT_800c3dc4)[DAT_800c3e7c * 6]);
  }
  return;
}


