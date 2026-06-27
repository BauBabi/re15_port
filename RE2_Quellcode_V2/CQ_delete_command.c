/* CQ_delete_command @ 0x80089ae4  (Ghidra headless, raw MIPS overlay) */

void CQ_delete_command(void)

{
  int iVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  
  iVar4 = (&DAT_800c3db8)[DAT_800c3e78 * 6];
  do {
    DAT_800c3e7c = DAT_800c3e78;
    if (DAT_800c3e80 < 1) {
      return;
    }
    iVar3 = 3;
    iVar1 = DAT_800c3e78 * 0x18;
    iVar2 = iVar1 + -0x7ff3c245;
    (&DAT_800c3db8)[DAT_800c3e78 * 6] = 0;
    (&DAT_800c3dbc)[iVar1] = 0;
    do {
      *(undefined1 *)(iVar2 + 5) = 0;
      iVar3 = iVar3 + -1;
      iVar2 = iVar2 + -1;
    } while (-1 < iVar3);
    (&DAT_800c3dc4)[DAT_800c3e7c * 6] = 0;
    (&DAT_800c3dc8)[DAT_800c3e7c * 6] = 0;
    (&DAT_800c3dcc)[DAT_800c3e7c * 6] = 0;
    DAT_800c3e78 = DAT_800c3e78 + 1;
    if (7 < DAT_800c3e78) {
      DAT_800c3e78 = 0;
    }
    DAT_800c3e80 = DAT_800c3e80 + -1;
  } while ((&DAT_800c3db8)[DAT_800c3e78 * 6] == iVar4);
  DAT_800c3e7c = DAT_800c3e78;
  return;
}


