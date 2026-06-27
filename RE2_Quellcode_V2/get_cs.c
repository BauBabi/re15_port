/* get_cs @ 0x80091804  (Ghidra headless, raw MIPS overlay) */

uint get_cs(short param_1,uint param_2)

{
  uint uVar1;
  int iVar2;
  
  if (param_1 < 0) {
    uVar1 = 0;
  }
  else {
    uVar1 = (int)DAT_800b2704 - 1;
    if ((int)param_1 <= DAT_800b2704 + -1) {
      uVar1 = SYS_OBJ_1964();
      return uVar1;
    }
  }
  if ((short)param_2 < 0) {
    param_2 = 0;
  }
  else {
    iVar2 = (int)DAT_800b2706;
    if (iVar2 + -1 < (int)(short)param_2) {
      uVar1 = SYS_OBJ_19A4(iVar2,iVar2 + -1);
      return uVar1;
    }
  }
  if (1 < DAT_800b2700 - 1) {
    uVar1 = SYS_OBJ_19D8();
    return uVar1;
  }
  return (param_2 & 0xfff) << 0xc | uVar1 & 0xfff | 0xe3000000;
}


