/* SYS_OBJ_1964 @ 0x80091844  (Ghidra headless, raw MIPS overlay) */

uint SYS_OBJ_1964(undefined4 param_1,uint param_2)

{
  uint in_v0;
  int iVar1;
  uint uVar2;
  
  if ((short)param_2 < 0) {
    param_2 = 0;
  }
  else {
    iVar1 = (int)DAT_800b2706;
    if (iVar1 + -1 < (int)(short)param_2) {
      uVar2 = SYS_OBJ_19A4(iVar1,iVar1 + -1);
      return uVar2;
    }
  }
  if (1 < DAT_800b2700 - 1) {
    uVar2 = SYS_OBJ_19D8();
    return uVar2;
  }
  return (param_2 & 0xfff) << 0xc | in_v0 & 0xfff | 0xe3000000;
}


