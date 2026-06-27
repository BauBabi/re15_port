undefined4 FUN_80013a88(u_char param_1,u_char *param_2,u_char *param_3)

{
  int iVar1;
  
  do {
    iVar1 = CdSync(1,(u_char *)0x0);
  } while (iVar1 == 0);
  do {
    iVar1 = CdControl('\x01',(u_char *)0x0,"");
  } while (iVar1 == 0);
  DAT_800b2b38 = 0;
  DAT_8008f614 = 0;
  do {
    iVar1 = CdControlB(param_1,param_2,param_3);
    if (iVar1 != 0) {
      return 1;
    }
    DAT_8008f614 = DAT_8008f614 + 1;
  } while (DAT_8008f614 < 0x50);
  return 0;
}
