/* BIOS_OBJ_14A4 @ 0x800888d0  (Ghidra headless, raw MIPS overlay) */

undefined4 BIOS_OBJ_14A4(void)

{
  bool bVar1;
  int iVar2;
  int in_v0;
  undefined4 uVar3;
  int unaff_s0;
  byte *unaff_s1;
  int unaff_s2;
  int unaff_s3;
  
  while (((uVar3 = 0xffffffff, in_v0 == 0 &&
          (uVar3 = 0, (*(uint *)PTR_DMA_CDROM_CHCR_800ad0f8 & 0x1000000) != 0)) &&
         (uVar3 = 1, unaff_s2 == 0))) {
    iVar2 = VSync(-1);
    if ((DAT_800c3d58 < iVar2) ||
       (iVar2 = DAT_800c3d5c + 1, bVar1 = 0x3c0000 < DAT_800c3d5c, DAT_800c3d5c = iVar2, bVar1)) {
      FUN_8009881c("CD timeout: ");
      printf("%s:(%s) Sync=%s, Ready=%s\n",DAT_800c3d60,
             *(undefined4 *)((uint)DAT_800ace0d * 4 + unaff_s3),
             *(undefined4 *)((uint)*unaff_s1 * 4 + unaff_s0));
      CD_flush();
      uVar3 = BIOS_OBJ_14A4();
      return uVar3;
    }
    in_v0 = 0;
  }
  return uVar3;
}


