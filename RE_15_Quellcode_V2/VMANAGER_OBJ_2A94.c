undefined4 VMANAGER_OBJ_2A94(void)

{
  undefined2 uVar1;
  int in_v0;
  byte *pbVar2;
  undefined4 uVar3;
  uint uVar4;
  int iVar5;
  byte bVar6;
  byte bVar7;
  byte bVar8;
  undefined4 unaff_s2;
  short unaff_s3;
  short unaff_s4;
  int unaff_s5;
  undefined2 unaff_s6;
  undefined1 unaff_s7;
  undefined2 unaff_s8;
  
  pbVar2 = (byte *)((in_v0 >> 0x10) * 0x10 + DAT_800b2b28);
  DAT_800b531e = pbVar2[1];
  DAT_800b531f = pbVar2[4];
  DAT_800b5314 = *pbVar2;
  uVar3 = 0xffffffff;
  DAT_800b5319 = unaff_s7;
  if ((ushort)DAT_800b531b < *(ushort *)(DAT_800b2b34 + 0x12)) {
    bVar6 = 0;
    if (unaff_s5 == 0) {
      SpuVmKeyOff((int)unaff_s4,(int)unaff_s3,in_v0 >> 0x10,unaff_s6);
      uVar3 = VMANAGER_OBJ_2F20();
      return uVar3;
    }
    bVar8 = 0;
    bVar7 = bVar8;
    if (DAT_800b5314 != 0) {
      do {
        iVar5 = ((uint)DAT_800b531b * 0x10 + (uint)bVar6) * 0x20 + DAT_800b2b3c;
        bVar8 = bVar7;
        if ((*(byte *)(iVar5 + 6) <= DAT_800b5316) && (DAT_800b5316 <= *(byte *)(iVar5 + 7))) {
          bVar8 = bVar7 + 1;
          (&stack0x00000010)[bVar7] = *(undefined1 *)(iVar5 + 0x16);
          (&stack0x00000090)[bVar7] = bVar6;
        }
        bVar6 = bVar6 + 1;
        bVar7 = bVar8;
      } while (bVar6 < DAT_800b5314);
    }
    bVar6 = 0;
    uVar3 = unaff_s2;
    if (bVar8 != 0) {
      uVar4 = 0;
      do {
        DAT_800b532c = (ushort)(byte)(&stack0x00000010)[uVar4];
        DAT_800b5320 = (&stack0x00000090)[uVar4];
        pbVar2 = (byte *)(((uint)DAT_800b5320 + (uint)DAT_800b531b * 0x10) * 0x20 + DAT_800b2b3c);
        DAT_800b5323 = *pbVar2;
        DAT_800b5321 = pbVar2[2];
        DAT_800b5322 = pbVar2[3];
        DAT_800b5324 = pbVar2[4];
        DAT_800b5325 = pbVar2[5];
        DAT_800b5328 = pbVar2[1];
        DAT_800b5326 = pbVar2[6];
        DAT_800b5327 = pbVar2[7];
        uVar4 = SpuVmAlloc(0);
        uVar4 = uVar4 & 0xff;
        DAT_800b532e = (short)uVar4;
        if (uVar4 < DAT_800b52a8) {
          (&DAT_8008f85f)[uVar4 * 0x34] = 1;
          (&DAT_8008f846)[DAT_800b532e * 0x1a] = 0;
          (&DAT_8008f852)[DAT_800b532e * 0x1a] = unaff_s4;
          (&DAT_8008f85a)[DAT_800b532e * 0x1a] = (ushort)DAT_800b5315;
          (&DAT_8008f854)[DAT_800b532e * 0x1a] = (ushort)DAT_800b531b;
          (&DAT_8008f856)[DAT_800b532e * 0x1a] = unaff_s8;
          if (unaff_s4 != 0x21) {
            (&DAT_8008f84c)[DAT_800b532e * 0x1a] = (short)unaff_s5;
          }
          (&DAT_8008f84e)[DAT_800b532e * 0x34] = unaff_s7;
          (&DAT_8008f858)[DAT_800b532e * 0x1a] = (ushort)DAT_800b5320;
          (&DAT_8008f850)[DAT_800b532e * 0x1a] = unaff_s6;
          (&DAT_8008f85c)[DAT_800b532e * 0x1a] = (ushort)DAT_800b5323;
          (&DAT_8008f844)[DAT_800b532e * 0x1a] = DAT_800b532c;
          SpuVmDoAllocate();
          if (DAT_800b532c != 0xff) {
            uVar1 = note2pitch();
            SpuVmKeyOnNow(bVar8,uVar1);
            uVar3 = VMANAGER_OBJ_2F0C();
            return uVar3;
          }
          vmNoiseOn((undefined1)DAT_800b532e);
          uVar3 = VMANAGER_OBJ_2EF0();
          return uVar3;
        }
        uVar3 = 0xffffffff;
        bVar6 = bVar6 + 1;
        uVar4 = (uint)bVar6;
      } while (bVar6 < bVar8);
    }
  }
  return uVar3;
}
