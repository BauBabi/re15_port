undefined4 CD_datasync(int param_1)

{
  int iVar1;
  int iVar2;
  undefined4 uVar3;
  
  iVar1 = FUN_80061fc0(0xffffffff);
  DAT_8008fe10 = iVar1 + 0x3c0;
  DAT_8008fe14 = 0;
  DAT_8008fe18 = "CD_datasync";
  while (iVar2 = FUN_80061fc0(0xffffffff), iVar1 = DAT_8008fe14, iVar2 <= DAT_8008fe10) {
    DAT_8008fe14 = DAT_8008fe14 + 1;
    if (0x3c0000 < iVar1) break;
    uVar3 = 0;
    if (((DMA_CDROM_CHCR & 0x1000000) == 0) || (uVar3 = 1, param_1 != 0)) {
      return uVar3;
    }
  }
  puts("CD timeout: ");
  printf("%s:(%s) Sync=%s, Ready=%s\n",DAT_8008fe18,(&PTR_s_CdlSync_800788c8)[DAT_800788c5],
         (&PTR_s_NoIntr_80078948)[DAT_80078b80],(&PTR_s_NoIntr_80078948)[DAT_80078b81]);
  CD_flush();
  uVar3 = BIOS_OBJ_14C0();
  return uVar3;
}
