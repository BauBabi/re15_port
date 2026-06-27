/* SYS_OBJ_1DC0 @ 0x80091ca0  (Ghidra headless, raw MIPS overlay) */

undefined4 SYS_OBJ_1DC0(void)

{
  uint uVar1;
  undefined4 uVar2;
  ushort in_v1;
  ushort *in_t0;
  uint in_t1;
  
  in_t0[3] = in_v1;
  if (((*in_t0 & 0x3f) == 0) && ((in_t0[2] & 0x3f) == 0)) {
    DAT_800c3f18 = 0x5ffffff;
    DAT_800c3f1c = 0xe6000000;
    DAT_800c3f24 = in_t1 & 0xffffff | 0x2000000;
                    /* Probable PsyQ macro: setDrawTPage() if setlen(p, 1), setDrawMode() if
                       setlen(p, 2). */
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
                    /* Probable PsyQ macro: setDrawTPage() if setlen(p, 1), setDrawMode() if
                       setlen(p, 2). */
  DAT_800c3f2c = *(uint *)PTR_GPU_REG1_800b27d4 & 0x7ff | (in_t1 >> 0x1f) << 10 | 0xe1000000;
  DAT_800c3f34 = *(undefined4 *)in_t0;
  DAT_800c3f38 = *(undefined4 *)(in_t0 + 2);
  DAT_800c3f3c = 0x3ffffff;
  uVar1 = _param(3);
  DAT_800c3f40 = uVar1 | 0xe3000000;
  uVar1 = _param(4);
  DAT_800c3f44 = uVar1 | 0xe4000000;
  uVar1 = _param(5);
  DAT_800c3f48 = uVar1 | 0xe5000000;
  uVar2 = SYS_OBJ_1F64();
  return uVar2;
}


