/* SEQINIT_OBJ_1D8 @ 0x8007c8c0  (Ghidra headless, raw MIPS overlay) */

undefined4 SEQINIT_OBJ_1D8(void)

{
  int iVar1;
  undefined4 uVar2;
  undefined1 *puVar3;
  undefined1 *puVar4;
  short sVar5;
  uint uVar6;
  short in_t1;
  undefined4 *unaff_s0;
  uint uVar7;
  
  puVar3 = (undefined1 *)*unaff_s0;
  unaff_s0[0x25] = unaff_s0[0x23];
  *unaff_s0 = puVar3 + 1;
  puVar4 = (undefined1 *)*unaff_s0;
  *(undefined1 *)(unaff_s0 + 9) = *puVar3;
  *unaff_s0 = puVar4 + 1;
  *(undefined1 *)((int)unaff_s0 + 0x25) = *puVar4;
  uVar2 = _SsReadDeltaValue((int)in_t1,0);
  uVar7 = (int)*(short *)(unaff_s0 + 0x14) * unaff_s0[0x23];
  unaff_s0[2] = *unaff_s0;
  iVar1 = DAT_800ea228;
  unaff_s0[0x21] = uVar2;
  unaff_s0[0x24] = uVar2;
  unaff_s0[4] = 0;
  unaff_s0[3] = *unaff_s0;
  unaff_s0[1] = *unaff_s0;
  uVar6 = iVar1 * 0x3c;
  if (uVar7 * 10 < uVar6) {
    if (uVar7 == 0) {
      trap(0x1c00);
    }
    *(short *)((int)unaff_s0 + 0x52) = (short)((uint)(iVar1 * 600) / uVar7);
    uVar2 = SEQINIT_OBJ_328();
    return uVar2;
  }
  if (uVar6 == 0) {
    trap(0x1c00);
  }
  if (uVar6 == 0) {
    trap(0x1c00);
  }
  *(undefined2 *)((int)unaff_s0 + 0x52) = 0xffff;
  sVar5 = (short)((uint)((int)*(short *)(unaff_s0 + 0x14) * unaff_s0[0x23] * 10) / uVar6);
  *(short *)(unaff_s0 + 0x15) = sVar5;
  if ((uint)(iVar1 * 0x1e) < (uint)((int)*(short *)(unaff_s0 + 0x14) * unaff_s0[0x23] * 10) % uVar6)
  {
    *(short *)(unaff_s0 + 0x15) = sVar5 + 1;
  }
  *(undefined2 *)((int)unaff_s0 + 0x56) = *(undefined2 *)(unaff_s0 + 0x15);
  return 0;
}


