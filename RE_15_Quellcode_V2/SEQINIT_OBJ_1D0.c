undefined4 SEQINIT_OBJ_1D0(void)

{
  int iVar1;
  undefined4 uVar2;
  undefined2 uVar3;
  short sVar4;
  uint uVar5;
  short in_t1;
  int unaff_s0;
  uint uVar6;
  
  *(undefined4 *)(unaff_s0 + 0x8c) = *(undefined4 *)(unaff_s0 + 0x84);
  *(int *)(unaff_s0 + 4) = *(int *)(unaff_s0 + 4) + 2;
  uVar2 = _SsReadDeltaValue((int)in_t1,0);
  uVar6 = (int)*(short *)(unaff_s0 + 0x4a) * *(int *)(unaff_s0 + 0x84);
  *(undefined4 *)(unaff_s0 + 0xc) = *(undefined4 *)(unaff_s0 + 4);
  iVar1 = DAT_800bb4bc;
  *(undefined4 *)(unaff_s0 + 0x7c) = uVar2;
  *(undefined4 *)(unaff_s0 + 0x88) = uVar2;
  *(undefined4 *)(unaff_s0 + 8) = *(undefined4 *)(unaff_s0 + 4);
  uVar5 = iVar1 * 0x3c;
  if (uVar6 * 10 < uVar5) {
    if (uVar6 == 0) {
      trap(0x1c00);
    }
    uVar3 = (undefined2)((uint)(iVar1 * 600) / uVar6);
    *(undefined2 *)(unaff_s0 + 0x6e) = uVar3;
    *(undefined2 *)(unaff_s0 + 0x70) = uVar3;
    uVar2 = SEQINIT_OBJ_308();
    return uVar2;
  }
  if (uVar5 == 0) {
    trap(0x1c00);
  }
  if (uVar5 == 0) {
    trap(0x1c00);
  }
  *(undefined2 *)(unaff_s0 + 0x6e) = 0xffff;
  sVar4 = (short)((uint)((int)*(short *)(unaff_s0 + 0x4a) * *(int *)(unaff_s0 + 0x84) * 10) / uVar5)
  ;
  *(short *)(unaff_s0 + 0x70) = sVar4;
  if ((uint)(iVar1 * 0x1e) <
      (uint)((int)*(short *)(unaff_s0 + 0x4a) * *(int *)(unaff_s0 + 0x84) * 10) % uVar5) {
    *(short *)(unaff_s0 + 0x70) = sVar4 + 1;
  }
  *(undefined2 *)(unaff_s0 + 0x72) = *(undefined2 *)(unaff_s0 + 0x70);
  return 0;
}
