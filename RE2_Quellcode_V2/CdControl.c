/* CdControl @ 0x80086e5c  (Ghidra headless, raw MIPS overlay) */

int CdControl(u_char com,u_char *param,u_char *result)

{
  undefined4 uVar1;
  int iVar2;
  int iVar3;
  
  uVar1 = DAT_800acdec;
  iVar3 = 3;
  while( true ) {
    DAT_800acdec = 0;
    if ((com != 1) && ((DAT_800acdfc & 0x10) != 0)) {
      CD_cw(1,0,0,0);
    }
    if ((((param == (u_char *)0x0) || (*(int *)(&DAT_800acd64 + (uint)com * 4) == 0)) ||
        (iVar2 = CD_cw(2,param,result,0), iVar2 == 0)) &&
       (DAT_800acdec = uVar1, iVar2 = CD_cw(com,param,result,0), iVar2 == 0)) break;
    iVar3 = iVar3 + -1;
    if (iVar3 == -1) {
      DAT_800acdec = uVar1;
      return 0;
    }
  }
  return 1;
}


