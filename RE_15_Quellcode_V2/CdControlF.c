int CdControlF(u_char com,u_char *param)

{
  undefined4 uVar1;
  int iVar2;
  int iVar3;
  
  uVar1 = DAT_800788a4;
  iVar3 = 3;
  while( true ) {
    DAT_800788a4 = 0;
    if ((com != 1) && ((DAT_800788b4 & 0x10) != 0)) {
      CD_cw(1,0,0,0);
    }
    if ((((param == (u_char *)0x0) || (*(int *)(&DAT_80078824 + (uint)com * 4) == 0)) ||
        (iVar2 = CD_cw(2,param,0,0), iVar2 == 0)) &&
       (DAT_800788a4 = uVar1, iVar2 = CD_cw(com,param,0,1), iVar2 == 0)) break;
    iVar3 = iVar3 + -1;
    if (iVar3 == -1) {
      DAT_800788a4 = uVar1;
      return 0;
    }
  }
  return 1;
}
