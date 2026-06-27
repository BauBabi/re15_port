/* SYS_OBJ_1D84 @ 0x80091c64  (Ghidra headless, raw MIPS overlay) */

undefined4 SYS_OBJ_1D84(void)

{
  ushort in_v0;
  undefined4 uVar1;
  uint uVar2;
  ushort uVar3;
  int iVar4;
  ushort *in_t0;
  uint in_t1;
  
  iVar4 = (int)(short)in_t0[3];
  in_t0[2] = in_v0;
  if (iVar4 < 0) {
    uVar3 = 0;
  }
  else {
    uVar3 = DAT_800b2706 - 1;
    if (iVar4 <= DAT_800b2706 + -1) {
      uVar1 = SYS_OBJ_1DC0(iVar4);
      return uVar1;
    }
  }
  in_t0[3] = uVar3;
  if (((*in_t0 & 0x3f) == 0) && ((in_t0[2] & 0x3f) == 0)) {
    DAT_800c3f18 = 0x5ffffff;
    DAT_800c3f1c = 0xe6000000;
    DAT_800c3f24 = in_t1 & 0xffffff | 0x2000000;
    DAT_800c3f20 = *(uint *)PTR_GPU_REG1_800b27d4 & 0x7ff | (in_t1 >> 0x1f) << 10 | 0xe1000000;
    DAT_800c3f28 = *(undefined4 *)in_t0;
    DAT_800c3f2c = *(uint *)(in_t0 + 2);
    _cwc(&DAT_800c3f18);
    return 0;
  }
  DAT_800c3f18 = 0x80c3f3c;
  DAT_800c3f30 = in_t1 & 0xffffff | 0x60000000;
  DAT_800c3f28 = 0xe6000000;
  DAT_800c3f1c = 0xe3000000;
  DAT_800c3f20 = 0xe4ffffff;
  DAT_800c3f24 = 0xe5000000;
  DAT_800c3f2c = *(uint *)PTR_GPU_REG1_800b27d4 & 0x7ff | (in_t1 >> 0x1f) << 10 | 0xe1000000;
  DAT_800c3f34 = *(undefined4 *)in_t0;
  DAT_800c3f38 = *(undefined4 *)(in_t0 + 2);
  DAT_800c3f3c = 0x3ffffff;
  uVar2 = _param(3);
  DAT_800c3f40 = uVar2 | 0xe3000000;
  uVar2 = _param(4);
  DAT_800c3f44 = uVar2 | 0xe4000000;
  uVar2 = _param(5);
  DAT_800c3f48 = uVar2 | 0xe5000000;
  uVar1 = SYS_OBJ_1F64();
  return uVar1;
}


