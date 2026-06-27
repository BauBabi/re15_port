/* BIOS_OBJ_DDC @ 0x80088208  (Ghidra headless, raw MIPS overlay) */

int BIOS_OBJ_DDC(void)

{
  undefined1 uVar1;
  bool bVar2;
  uint uVar3;
  int iVar4;
  undefined1 *puVar5;
  byte unaff_s1;
  byte *unaff_s2;
  int unaff_s3;
  undefined1 *unaff_s4;
  undefined1 *unaff_s5;
  
  do {
    while( true ) {
      uVar3 = getintr();
      if (uVar3 == 0) break;
      if (((uVar3 & 4) != 0) && (DAT_800acdf0 != (code *)0x0)) {
        (*DAT_800acdf0)(*unaff_s4,&DAT_800c3d48);
      }
      if (((uVar3 & 2) != 0) && (DAT_800acdec != (code *)0x0)) {
        (*DAT_800acdec)(*unaff_s2,&DAT_800c3d40);
      }
    }
    *PTR_CDROM_REG0_800ad0b4 = unaff_s1;
    do {
      if (*unaff_s2 != 0) {
        puVar5 = &DAT_800c3d40;
        if (unaff_s5 != (undefined1 *)0x0) {
          iVar4 = 7;
          do {
            uVar1 = *puVar5;
            puVar5 = puVar5 + 1;
            iVar4 = iVar4 + -1;
            *unaff_s5 = uVar1;
            unaff_s5 = unaff_s5 + 1;
          } while (iVar4 != -1);
        }
        return -(uint)(DAT_800ad0cc == '\x05');
      }
      iVar4 = VSync(-1);
      if ((DAT_800c3d58 < iVar4) ||
         (iVar4 = DAT_800c3d5c + 1, bVar2 = 0x3c0000 < DAT_800c3d5c, DAT_800c3d5c = iVar4, bVar2)) {
        FUN_8009881c("CD timeout: ");
        printf("%s:(%s) Sync=%s, Ready=%s\n",DAT_800c3d60,(&PTR_s_CdlSync_800ace14)[DAT_800ace0d],
               *(undefined4 *)((uint)*unaff_s2 * 4 + unaff_s3));
        CD_flush();
        iVar4 = BIOS_OBJ_DAC();
        return iVar4;
      }
      iVar4 = CheckCallback();
    } while (iVar4 == 0);
    unaff_s1 = *PTR_CDROM_REG0_800ad0b4 & 3;
  } while( true );
}


