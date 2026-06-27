/* FUN_8003a6ac @ 0x8003a6ac  (Ghidra headless, raw MIPS overlay) */

void FUN_8003a6ac(int param_1)

{
  byte bVar1;
  byte bVar2;
  byte *pbVar3;
  byte *pbVar4;
  byte *pbVar5;
  byte *pbVar6;
  byte *pbVar7;
  
  pbVar3 = DAT_800ea23c;
  pbVar6 = DAT_800ea23c + 4;
  pbVar5 = DAT_800ea23c + 0x16c;
  pbVar4 = DAT_800ea23c + 0x34f;
  pbVar7 = DAT_800ea23c + 0x19cf;
  do {
    bVar1 = *pbVar6;
    pbVar6 = pbVar6 + 1;
    pbVar4[-0xffffffff0000000b] = 8;
    pbVar4[-0xffffffff0000000a] = 0;
    pbVar4[-0xffffffff00000009] = 10;
    pbVar4[-0xffffffff00000008] = 0;
    bVar2 = *pbVar5;
    *pbVar4 = (byte)param_1;
    *(uint *)(pbVar4 + -7) =
         (((bVar1 - 0x20 & 0xffff) >> 5) * 0xa00 & 0xffff | (bVar1 - 0x20 & 0x1f) << 3) +
         (bVar2 + 0x7b40) * 0x10000;
    pbVar4 = pbVar4 + 0x10;
    pbVar5 = pbVar5 + 1;
  } while (pbVar4 < pbVar7);
  *(ushort *)(pbVar3 + 800) = (ushort)*pbVar3 * 8 + 0x10;
  *(ushort *)(pbVar3 + 0x322) = (ushort)pbVar3[1] * 10 + 0x15;
  pbVar3[0x2d5] = pbVar3[0x2d5] + 1 & 0x1f;
  *(ushort *)(pbVar3 + 0x32a) = pbVar3[0x2d7] + 0x7b40;
  if (param_1 == 0) {
    pbVar3[0x32f] = 0;
  }
  else {
    pbVar3[0x32f] = pbVar3[0x2d5] & 0x10;
  }
  pbVar3[0x33f] = (byte)param_1;
  return;
}


