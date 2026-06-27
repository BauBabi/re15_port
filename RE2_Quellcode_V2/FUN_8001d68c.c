/* FUN_8001d68c @ 0x8001d68c  (Ghidra headless, raw MIPS overlay) */

void FUN_8001d68c(int param_1)

{
  byte *pbVar1;
  byte bVar2;
  byte bVar3;
  byte bVar4;
  byte *pbVar5;
  byte *pbVar6;
  
  if (param_1 != 0) {
    (*(code *)(&PTR_FUN_8009d868)[*DAT_800dcbd0])();
  }
  FUN_8001d894();
  (*(code *)(&PTR_FUN_8009d868)[DAT_800dcbd0[1]])();
  pbVar5 = DAT_800dcbd0;
  if ((*(ushort *)(DAT_800dcbd0 + 0x18) & 2) != 0) {
    bVar2 = DAT_800dcbd0[8];
    bVar3 = DAT_800dcbd0[9];
    bVar4 = DAT_800dcbd0[10];
    pbVar6 = DAT_800dcbd0 + 0x26;
    pbVar1 = DAT_800dcbd0 + 0xe;
    *(short *)(DAT_800dcbd0 + 0x24) =
         *(short *)(DAT_800dcbd0 + 0x24) + *(short *)(DAT_800dcbd0 + 0xc);
    *(short *)(pbVar5 + 0x26) = *(short *)pbVar6 + *(short *)pbVar1;
    *(short *)(pbVar5 + 0x28) = *(short *)(pbVar5 + 0x28) + *(short *)(pbVar5 + 0x10);
    *(short *)(pbVar5 + 0xc) = *(short *)(pbVar5 + 0xc) + (short)(char)bVar2;
    *(short *)(pbVar5 + 0xe) = *(short *)(pbVar5 + 0xe) + (short)(char)bVar3;
    *(short *)(pbVar5 + 0x10) = *(short *)(pbVar5 + 0x10) + (short)(char)bVar4;
  }
  if ((*(ushort *)(DAT_800dcbd0 + 0x18) & 1) != 0) {
    if (DAT_800dcbd0[0x20] == 0) {
      DAT_800dcbd0[0x21] = DAT_800dcbd0[0x21] + ((byte)*(ushort *)(DAT_800dcbd0 + 0x18) & 1);
      pbVar5 = DAT_800dcbd0;
      pbVar6 = (byte *)(*(int *)(DAT_800dcbd0 + 0x70) + (uint)DAT_800dcbd0[0x21] * 8);
      if (pbVar6[2] == 0) {
        if (*pbVar6 == 0) {
          DAT_800dcbd0[1] = 0;
          *pbVar5 = 0;
          pbVar5[0x18] = 0;
          pbVar5[0x19] = 0;
        }
      }
      else if (pbVar6[2] == 0xff) {
        DAT_800dcbd0[0x21] = *pbVar6;
        pbVar6 = (byte *)(*(int *)(DAT_800dcbd0 + 0x70) + (uint)DAT_800dcbd0[0x21] * 8);
      }
      DAT_800dcbd0[0x20] = pbVar6[2];
    }
    DAT_800dcbd0[0x20] = DAT_800dcbd0[0x20] - 1;
  }
  return;
}


