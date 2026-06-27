undefined4 BIOS_OBJ_14C0(void)

{
  bool bVar1;
  int iVar2;
  int in_v0;
  undefined4 uVar3;
  int unaff_s0;
  byte *unaff_s1;
  int unaff_s2;
  int unaff_s3;
  
  while (((uVar3 = 0xffffffff, in_v0 == 0 && (uVar3 = 0, (DMA_CDROM_CHCR & 0x1000000) != 0)) &&
         (uVar3 = 1, unaff_s2 == 0))) {
    iVar2 = FUN_80061fc0(0xffffffff);
    if ((DAT_8008fe10 < iVar2) ||
       (iVar2 = DAT_8008fe14 + 1, bVar1 = 0x3c0000 < DAT_8008fe14, DAT_8008fe14 = iVar2, bVar1)) {
      puts("CD timeout: ");
      printf("%s:(%s) Sync=%s, Ready=%s\n",DAT_8008fe18,
             *(undefined4 *)((uint)DAT_800788c5 * 4 + unaff_s3),
             *(undefined4 *)((uint)*unaff_s1 * 4 + unaff_s0));
      CD_flush();
      uVar3 = BIOS_OBJ_14C0();
      return uVar3;
    }
    in_v0 = 0;
  }
  return uVar3;
}
