/* FUN_8003a148 @ 0x8003a148  (Ghidra headless, raw MIPS overlay) */

undefined4 FUN_8003a148(int param_1,int param_2,uint param_3,uint param_4)

{
  int iVar1;
  uint uVar2;
  
  iVar1 = DAT_800ea23c;
  if (DAT_800d4237 == '\0') {
    uVar2 = (uint)*(byte *)(DAT_800ea23c + 0x2dc);
    if (uVar2 < 0xd) {
      FUN_8003a034(param_1,param_2,param_1 + uVar2 * (param_3 / 0xc),
                   param_2 + uVar2 * (param_4 / 0xc));
      *(char *)(iVar1 + 0x2dc) = *(char *)(iVar1 + 0x2dc) + '\x01';
      return 1;
    }
  }
  else {
    if (DAT_800d4237 != '\x01') {
      return 1;
    }
    uVar2 = (uint)*(byte *)(DAT_800ea23c + 0x2dc);
    if (uVar2 < 0xd) {
      FUN_8003a034(param_1,param_2,(param_1 + param_3) - (uVar2 * param_3) / 0xc,
                   (param_2 + param_4) - (uVar2 * param_4) / 0xc);
      *(char *)(iVar1 + 0x2dc) = *(char *)(iVar1 + 0x2dc) + '\x01';
      return 1;
    }
  }
  *(undefined1 *)(DAT_800ea23c + 0x2dc) = 0;
  DAT_800d4237 = 0;
  return 0;
}


