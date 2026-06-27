/* FUN_8001d300 @ 0x8001d300  (Ghidra headless, raw MIPS overlay) */

void FUN_8001d300(void)

{
  byte bVar1;
  byte *pbVar2;
  uint uVar3;
  
  if ((DAT_800cfbdc & 0x10000000) == 0) {
    DAT_800dcbd0 = &DAT_800d8cf0;
    do {
      pbVar2 = DAT_800dcbd0;
      if ((*(ushort *)(DAT_800dcbd0 + 0x18) & 0x8000) != 0) {
        if ((DAT_800dcbd0[0x1a] != 0) && ((&DAT_800d8d08)[(uint)DAT_800dcbd0[0x1a] * 0x3e] == 0)) {
          DAT_800dcbd0[1] = 0;
          *pbVar2 = 0;
          pbVar2[0x18] = 0;
          pbVar2[0x19] = 0;
        }
        (*(code *)(&PTR_FUN_8009d868)[*DAT_800dcbd0])();
      }
      DAT_800dcbd0 = DAT_800dcbd0 + 0x7c;
    } while (DAT_800dcbd0 < &DAT_800dbb70);
    DAT_800dcbd0 = &DAT_800d8cf0;
    do {
      if ((*(ushort *)(DAT_800dcbd0 + 0x18) & 0x4000) != 0) {
        bVar1 = *DAT_800dcbd0;
        pbVar2 = DAT_800dcbd0;
        pbVar2[0x18] = 3;
        pbVar2[0x19] = 0xa0;
        (*(code *)(&PTR_FUN_8009d868)[bVar1])();
      }
      if ((*(ushort *)(DAT_800dcbd0 + 0x18) & 0x8000) != 0) {
        FUN_8001d68c(0);
      }
      DAT_800dcbd0 = DAT_800dcbd0 + 0x7c;
    } while (DAT_800dcbd0 < &DAT_800dbb70);
  }
  else {
    DAT_800dcbd0 = &DAT_800d8cf0;
    do {
      pbVar2 = DAT_800dcbd0;
      uVar3 = (uint)DAT_800dcbd0[0x1c];
      if (((((uVar3 == 0x1a) || (uVar3 == 0x1c)) || (uVar3 == 0x28)) ||
          ((uVar3 - 0x15 < 2 || (uVar3 == 1)))) &&
         ((*(ushort *)(DAT_800dcbd0 + 0x18) & 0x8000) != 0)) {
        if ((DAT_800dcbd0[0x1a] != 0) && ((&DAT_800d8d08)[(uint)DAT_800dcbd0[0x1a] * 0x3e] == 0)) {
          DAT_800dcbd0[1] = 0;
          *pbVar2 = 0;
          pbVar2[0x18] = 0;
          pbVar2[0x19] = 0;
        }
        (*(code *)(&PTR_FUN_8009d868)[*DAT_800dcbd0])();
      }
      DAT_800dcbd0 = DAT_800dcbd0 + 0x7c;
    } while (DAT_800dcbd0 < &DAT_800dbb70);
    DAT_800dcbd0 = &DAT_800d8cf0;
    do {
      uVar3 = (uint)DAT_800dcbd0[0x1c];
      if ((((uVar3 == 0x1a) || (uVar3 == 0x1c)) || (uVar3 == 0x28)) || (uVar3 - 0x15 < 2)) {
        if ((*(ushort *)(DAT_800dcbd0 + 0x18) & 0x4000) != 0) {
          bVar1 = *DAT_800dcbd0;
          pbVar2 = DAT_800dcbd0;
          pbVar2[0x18] = 3;
          pbVar2[0x19] = 0xa0;
          (*(code *)(&PTR_FUN_8009d868)[bVar1])();
        }
        if ((*(ushort *)(DAT_800dcbd0 + 0x18) & 0x8000) != 0) {
          FUN_8001d68c(0);
        }
      }
      DAT_800dcbd0 = DAT_800dcbd0 + 0x7c;
    } while (DAT_800dcbd0 <= &UNK_800dbb6f);
  }
  return;
}


