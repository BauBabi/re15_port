void FUN_8002877c(void)

{
  byte bVar1;
  byte *pbVar2;
  int iVar3;
  
  pbVar2 = (byte *)((int)DAT_800b852c + 1);
  iVar3 = (uint)*pbVar2 * (DAT_800b8520 & 1);
  bVar1 = pbVar2[iVar3 + 1];
  DAT_800b852c = pbVar2 + iVar3 + 2;
  if (((bVar1 != 0xf8) && (0xf8 < bVar1)) && (bVar1 == 0xf9)) {
    (*(code *)(&PTR_LAB_80072f34)[*DAT_800b852c])();
  }
  return;
}
