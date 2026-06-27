/* _clr @ 0x80091c14  (Ghidra headless, raw MIPS overlay) */

undefined4 _clr(ushort *param_1,uint param_2)

{
  ushort uVar1;
  undefined4 uVar2;
  uint uVar3;
  int iVar4;
  
  iVar4 = (int)(short)param_1[2];
  if (iVar4 < 0) {
    uVar1 = 0;
  }
  else {
    uVar1 = DAT_800b2704 - 1;
    if (iVar4 <= DAT_800b2704 + -1) {
      uVar2 = SYS_OBJ_1D84(iVar4,iVar4);
      return uVar2;
    }
  }
  iVar4 = (int)(short)param_1[3];
  param_1[2] = uVar1;
  if (iVar4 < 0) {
    uVar1 = 0;
  }
  else {
    uVar1 = DAT_800b2706 - 1;
    if (iVar4 <= DAT_800b2706 + -1) {
      uVar2 = SYS_OBJ_1DC0(iVar4);
      return uVar2;
    }
  }
  param_1[3] = uVar1;
  if (((*param_1 & 0x3f) == 0) && ((param_1[2] & 0x3f) == 0)) {
    DAT_800c3f18 = 0x5ffffff;
    DAT_800c3f1c = 0xe6000000;
    DAT_800c3f24 = param_2 & 0xffffff | 0x2000000;
    DAT_800c3f20 = *(uint *)PTR_GPU_REG1_800b27d4 & 0x7ff | (param_2 >> 0x1f) << 10 | 0xe1000000;
    DAT_800c3f28 = *(undefined4 *)param_1;
    DAT_800c3f2c = *(uint *)(param_1 + 2);
    _cwc(&DAT_800c3f18);
    return 0;
  }
  DAT_800c3f18 = 0x80c3f3c;
  DAT_800c3f30 = param_2 & 0xffffff | 0x60000000;
  DAT_800c3f28 = 0xe6000000;
  DAT_800c3f1c = 0xe3000000;
  DAT_800c3f20 = 0xe4ffffff;
  DAT_800c3f24 = 0xe5000000;
  DAT_800c3f2c = *(uint *)PTR_GPU_REG1_800b27d4 & 0x7ff | (param_2 >> 0x1f) << 10 | 0xe1000000;
  DAT_800c3f34 = *(undefined4 *)param_1;
  DAT_800c3f38 = *(undefined4 *)(param_1 + 2);
  DAT_800c3f3c = 0x3ffffff;
  uVar3 = _param(3);
  DAT_800c3f40 = uVar3 | 0xe3000000;
  uVar3 = _param(4);
  DAT_800c3f44 = uVar3 | 0xe4000000;
  uVar3 = _param(5);
  DAT_800c3f48 = uVar3 | 0xe5000000;
  uVar2 = SYS_OBJ_1F64();
  return uVar2;
}


