/* DsReady @ 0x8008aac4  (Ghidra headless, raw MIPS overlay) */

int DsReady(u_char *result)

{
  int iVar1;
  uint uVar2;
  char cVar3;
  
  DS_ready(0);
  cVar3 = '\x04';
  if (DAT_800c3da8 != 1) {
    cVar3 = DAT_800c3d98 == 1;
  }
  if (cVar3 == '\x04') {
    DAT_800c3da8 = 0;
    rescpy(result,&DAT_800c3dad);
    iVar1 = DSSYS_2_OBJ_10AC();
    return iVar1;
  }
  uVar2 = 0;
  if (cVar3 == '\x01') {
    DAT_800c3d98 = 0;
    rescpy(result,&DAT_800c3d9d);
    uVar2 = (uint)DAT_800c3d9c;
  }
  return uVar2;
}


