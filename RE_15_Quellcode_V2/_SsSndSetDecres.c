void _SsSndSetDecres(int param_1,short param_2,short param_3,uint param_4)

{
  uint uVar1;
  int iVar2;
  int *piVar3;
  
  iVar2 = param_2 * 0xac + *(int *)((int)&DAT_800bb500 + ((param_1 << 0x10) >> 0xe));
  if ((((*(uint *)(iVar2 + 0x90) & 4) == 0) && ((*(uint *)(iVar2 + 0x90) & 0x100) == 0)) &&
     (uVar1 = (uint)param_3, uVar1 != 0)) {
    if ((int)uVar1 < 0) {
      uVar1 = -uVar1;
    }
    *(short *)(iVar2 + 0x3e) = param_3;
    *(uint *)(iVar2 + 0x94) = param_4;
    *(short *)(iVar2 + 0x40) = param_3;
    *(uint *)(iVar2 + 0x98) = param_4;
    if (uVar1 <= param_4) {
      if (uVar1 == 0) {
        trap(0x1c00);
      }
      *(short *)(iVar2 + 0x42) = (short)(param_4 / uVar1);
      VOL_OBJ_348();
      return;
    }
    if (param_4 == 0) {
      trap(0x1c00);
    }
    *(short *)(iVar2 + 0x42) = -(short)(uVar1 / param_4);
  }
  piVar3 = (int *)((int)&DAT_800bb500 + ((param_1 << 0x10) >> 0xe));
  iVar2 = param_2 * 0xac + *piVar3;
  *(uint *)(iVar2 + 0x90) = *(uint *)(iVar2 + 0x90) | 0x20;
  iVar2 = param_2 * 0xac + *piVar3;
  *(uint *)(iVar2 + 0x90) = *(uint *)(iVar2 + 0x90) & 0xffffffef;
  return;
}
