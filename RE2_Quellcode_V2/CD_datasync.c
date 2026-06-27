/* CD_datasync @ 0x800887b4  (Ghidra headless, raw MIPS overlay) */

undefined4 CD_datasync(int param_1)

{
  int iVar1;
  int iVar2;
  undefined4 uVar3;
  
  iVar1 = VSync(-1);
  DAT_800c3d58 = iVar1 + 0x3c0;
  DAT_800c3d5c = 0;
  DAT_800c3d60 = "CD_datasync";
  while (iVar2 = VSync(-1), iVar1 = DAT_800c3d5c, iVar2 <= DAT_800c3d58) {
    DAT_800c3d5c = DAT_800c3d5c + 1;
    if (0x3c0000 < iVar1) break;
    uVar3 = 0;
    if (((*(uint *)PTR_DMA_CDROM_CHCR_800ad0f8 & 0x1000000) == 0) || (uVar3 = 1, param_1 != 0)) {
      return uVar3;
    }
  }
  FUN_8009881c("CD timeout: ");
  printf("%s:(%s) Sync=%s, Ready=%s\n",DAT_800c3d60,(&PTR_s_CdlSync_800ace14)[DAT_800ace0d],
         (&PTR_s_NoIntr_800ace94)[DAT_800ad0cc],(&PTR_s_NoIntr_800ace94)[DAT_800ad0cd]);
  CD_flush();
  uVar3 = BIOS_OBJ_14A4();
  return uVar3;
}


