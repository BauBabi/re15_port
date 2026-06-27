undefined4 VMANAGER_OBJ_2F0C(void)

{
  undefined2 uVar1;
  uint uVar2;
  undefined4 uVar3;
  undefined1 *puVar4;
  uint uVar5;
  ushort *unaff_s0;
  uint unaff_s1;
  undefined4 unaff_s2;
  uint unaff_s3;
  short unaff_s4;
  undefined2 unaff_s5;
  undefined2 unaff_s6;
  undefined1 unaff_s7;
  undefined2 unaff_s8;
  
  while( true ) {
    unaff_s1 = unaff_s1 + 1;
    if (unaff_s3 <= (unaff_s1 & 0xff)) {
      return unaff_s2;
    }
    *unaff_s0 = (ushort)(byte)(&stack0x00000010)[unaff_s1 & 0xff];
    *(undefined1 *)(unaff_s0 + -6) = (&stack0x00000090)[unaff_s1 & 0xff];
    puVar4 = (undefined1 *)
             (((uint)(byte)unaff_s0[-6] + (uint)*(byte *)((int)unaff_s0 + -0x11) * 0x10) * 0x20 +
             DAT_800b2b3c);
    *(undefined1 *)((int)unaff_s0 + -9) = *puVar4;
    *(undefined1 *)((int)unaff_s0 + -0xb) = puVar4[2];
    *(undefined1 *)(unaff_s0 + -5) = puVar4[3];
    *(undefined1 *)(unaff_s0 + -4) = puVar4[4];
    *(undefined1 *)((int)unaff_s0 + -7) = puVar4[5];
    *(undefined1 *)(unaff_s0 + -2) = puVar4[1];
    *(undefined1 *)(unaff_s0 + -3) = puVar4[6];
    *(undefined1 *)((int)unaff_s0 + -5) = puVar4[7];
    uVar2 = SpuVmAlloc(0);
    uVar2 = uVar2 & 0xff;
    uVar5 = (uint)DAT_800b52a8;
    unaff_s0[1] = (ushort)uVar2;
    if (uVar2 < uVar5) break;
    unaff_s2 = 0xffffffff;
  }
  (&DAT_8008f85f)[uVar2 * 0x34] = 1;
  (&DAT_8008f846)[(short)unaff_s0[1] * 0x1a] = 0;
  (&DAT_8008f852)[(short)unaff_s0[1] * 0x1a] = unaff_s4;
  (&DAT_8008f85a)[(short)unaff_s0[1] * 0x1a] = (ushort)*(byte *)((int)unaff_s0 + -0x17);
  (&DAT_8008f854)[(short)unaff_s0[1] * 0x1a] = (ushort)*(byte *)((int)unaff_s0 + -0x11);
  (&DAT_8008f856)[(short)unaff_s0[1] * 0x1a] = unaff_s8;
  if (unaff_s4 != 0x21) {
    (&DAT_8008f84c)[(short)unaff_s0[1] * 0x1a] = unaff_s5;
  }
  (&DAT_8008f84e)[(short)unaff_s0[1] * 0x34] = unaff_s7;
  (&DAT_8008f858)[(short)unaff_s0[1] * 0x1a] = (ushort)(byte)unaff_s0[-6];
  (&DAT_8008f850)[(short)unaff_s0[1] * 0x1a] = unaff_s6;
  (&DAT_8008f85c)[(short)unaff_s0[1] * 0x1a] = (ushort)*(byte *)((int)unaff_s0 + -9);
  (&DAT_8008f844)[(short)unaff_s0[1] * 0x1a] = *unaff_s0;
  SpuVmDoAllocate();
  if (*unaff_s0 == 0xff) {
    vmNoiseOn((undefined1)DAT_800b532e);
    uVar3 = VMANAGER_OBJ_2EF0();
    return uVar3;
  }
  uVar1 = note2pitch();
  SpuVmKeyOnNow(unaff_s3 & 0xff,uVar1);
  uVar3 = VMANAGER_OBJ_2F0C();
  return uVar3;
}
