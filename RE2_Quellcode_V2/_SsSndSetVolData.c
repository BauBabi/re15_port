/* _SsSndSetVolData @ 0x8007a188  (Ghidra headless, raw MIPS overlay) */

void _SsSndSetVolData(int param_1,short param_2,short param_3,int param_4)

{
  int iVar1;
  int iVar2;
  
  iVar2 = *(int *)((int)&DAT_800ea250 + ((param_1 << 0x10) >> 0xe)) + param_2 * 0xb0;
  if ((((*(uint *)(iVar2 + 0x98) & 4) == 0) && ((*(uint *)(iVar2 + 0x98) & 0x100) == 0)) &&
     (iVar1 = (int)param_3, iVar1 != 0)) {
    if (iVar1 < 0) {
      iVar1 = -iVar1;
    }
    *(short *)(iVar2 + 0x48) = param_3;
    *(int *)(iVar2 + 0x9c) = param_4;
    *(short *)(iVar2 + 0x4a) = param_3;
    *(int *)(iVar2 + 0xa0) = param_4;
    if (iVar1 <= param_4) {
      if (iVar1 == 0) {
        trap(0x1c00);
      }
      if ((iVar1 == -1) && (param_4 == -0x80000000)) {
        trap(0x1800);
      }
      *(short *)(iVar2 + 0x4c) = (short)(param_4 / iVar1);
      VOL_OBJ_F0();
      return;
    }
    if (param_4 == 0) {
      trap(0x1c00);
    }
    if ((param_4 == -1) && (iVar1 == -0x80000000)) {
      trap(0x1800);
    }
    *(short *)(iVar2 + 0x4c) = -(short)(iVar1 / param_4);
  }
  return;
}


