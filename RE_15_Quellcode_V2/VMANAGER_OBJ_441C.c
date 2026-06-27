int VMANAGER_OBJ_441C(void)

{
  byte bVar1;
  undefined2 uVar2;
  int in_v0;
  undefined1 *puVar3;
  int iVar4;
  ushort unaff_s2;
  undefined2 unaff_s5;
  undefined2 unaff_s7;
  undefined2 in_stack_00000010;
  
  puVar3 = (undefined1 *)((in_v0 >> 0xc) + DAT_800b2b28);
  DAT_800b531e = puVar3[1];
  DAT_800b531f = puVar3[4];
  DAT_800b5314 = *puVar3;
  puVar3 = (undefined1 *)(((uint)DAT_800b5320 + (uint)DAT_800b531b * 0x10) * 0x20 + DAT_800b2b3c);
  DAT_800b5323 = *puVar3;
  DAT_800b532c = *(short *)(puVar3 + 0x16);
  DAT_800b5321 = puVar3[2];
  DAT_800b5322 = puVar3[3];
  DAT_800b5324 = puVar3[4];
  DAT_800b5325 = puVar3[5];
  DAT_800b5328 = puVar3[1];
  DAT_800b5326 = puVar3[6];
  DAT_800b5327 = puVar3[7];
  if (DAT_800b532c == 0) {
    DAT_800b283c = 0;
    iVar4 = VMANAGER_OBJ_45FC();
    return iVar4;
  }
  iVar4 = (int)(short)unaff_s2;
  DAT_800b532e = unaff_s2;
  (&DAT_8008f852)[iVar4 * 0x1a] = 0x21;
  (&DAT_8008f85a)[iVar4 * 0x1a] = in_stack_00000010;
  bVar1 = DAT_800b531b;
  (&DAT_8008f856)[iVar4 * 0x1a] = unaff_s7;
  (&DAT_8008f854)[iVar4 * 0x1a] = (ushort)bVar1;
  (&DAT_8008f844)[iVar4 * 0x1a] = DAT_800b532c;
  bVar1 = DAT_800b5320;
  (&DAT_8008f850)[iVar4 * 0x1a] = unaff_s5;
  (&DAT_8008f85f)[iVar4 * 0x34] = 1;
  (&DAT_8008f846)[iVar4 * 0x1a] = 0;
  (&DAT_8008f858)[iVar4 * 0x1a] = (ushort)bVar1;
  SpuVmDoAllocate();
  if (DAT_800b532c == 0xff) {
    vmNoiseOn(unaff_s2 & 0xff);
    iVar4 = VMANAGER_OBJ_45EC();
    return iVar4;
  }
  uVar2 = note2pitch2();
  SpuVmKeyOnNow(1,uVar2);
  DAT_800b283c = 0;
  return (int)(short)unaff_s2;
}
