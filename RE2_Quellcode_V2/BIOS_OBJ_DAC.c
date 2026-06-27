/* BIOS_OBJ_DAC @ 0x800881d8  (Ghidra headless, raw MIPS overlay) */

int BIOS_OBJ_DAC(void)

{
  byte bVar1;
  undefined1 uVar2;
  bool bVar3;
  int in_v0;
  int iVar4;
  uint uVar5;
  undefined1 *puVar6;
  byte *unaff_s2;
  int unaff_s3;
  undefined1 *unaff_s4;
  undefined1 *unaff_s5;
  
  while( true ) {
    if (in_v0 != 0) {
      return -1;
    }
    iVar4 = CheckCallback();
    if (iVar4 != 0) {
      bVar1 = *PTR_CDROM_REG0_800ad0b4;
      while( true ) {
        uVar5 = getintr();
        if (uVar5 == 0) break;
        if (((uVar5 & 4) != 0) && (DAT_800acdf0 != (code *)0x0)) {
          (*DAT_800acdf0)(*unaff_s4,&DAT_800c3d48);
        }
        if (((uVar5 & 2) != 0) && (DAT_800acdec != (code *)0x0)) {
          (*DAT_800acdec)(*unaff_s2,&DAT_800c3d40);
        }
      }
      *PTR_CDROM_REG0_800ad0b4 = bVar1 & 3;
    }
    if (*unaff_s2 != 0) break;
    iVar4 = VSync(-1);
    if ((DAT_800c3d58 < iVar4) ||
       (iVar4 = DAT_800c3d5c + 1, bVar3 = 0x3c0000 < DAT_800c3d5c, DAT_800c3d5c = iVar4, bVar3)) {
      FUN_8009881c("CD timeout: ");
      printf("%s:(%s) Sync=%s, Ready=%s\n",DAT_800c3d60,(&PTR_s_CdlSync_800ace14)[DAT_800ace0d],
             *(undefined4 *)((uint)*unaff_s2 * 4 + unaff_s3));
      CD_flush();
      iVar4 = BIOS_OBJ_DAC();
      return iVar4;
    }
    in_v0 = 0;
  }
  puVar6 = &DAT_800c3d40;
  if (unaff_s5 != (undefined1 *)0x0) {
    iVar4 = 7;
    do {
      uVar2 = *puVar6;
      puVar6 = puVar6 + 1;
      iVar4 = iVar4 + -1;
      *unaff_s5 = uVar2;
      unaff_s5 = unaff_s5 + 1;
    } while (iVar4 != -1);
  }
  return -(uint)(DAT_800ad0cc == '\x05');
}


