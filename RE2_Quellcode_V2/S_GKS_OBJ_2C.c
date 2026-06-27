/* S_GKS_OBJ_2C @ 0x800796c0  (Ghidra headless, raw MIPS overlay) */

int S_GKS_OBJ_2C(undefined4 param_1,uint param_2)

{
  int iVar1;
  
  if (param_2 == 0xffffffff) {
    iVar1 = S_GKS_OBJ_8C();
    return iVar1;
  }
  if ((DAT_800ab284 & 1 << (param_2 & 0x1f)) == 0) {
    iVar1 = (uint)(*(short *)(PTR_VOICE_00_LEFT_RIGHT_800ab21c + param_2 * 0x10 + 0xc) != 0) << 1;
  }
  else {
    iVar1 = 3;
    if (*(short *)(PTR_VOICE_00_LEFT_RIGHT_800ab21c + param_2 * 0x10 + 0xc) != 0) {
      iVar1 = S_GKS_OBJ_8C();
      return iVar1;
    }
  }
  return iVar1;
}


