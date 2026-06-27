/* CQ_execute @ 0x80089de0  (Ghidra headless, raw MIPS overlay) */

undefined4 CQ_execute(void)

{
  int iVar1;
  undefined4 uVar2;
  
  iVar1 = DS_system_status(0);
  if ((iVar1 == 1) && ((&DAT_800c3db8)[DAT_800c3e7c * 6] != 0)) {
    DS_cw((&DAT_800c3dbc)[DAT_800c3e7c * 0x18],(&DAT_800c3dc4)[DAT_800c3e7c * 6]);
    uVar2 = DSSYS_2_OBJ_394();
    return uVar2;
  }
  return 0;
}


