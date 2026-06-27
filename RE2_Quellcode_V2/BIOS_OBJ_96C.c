/* BIOS_OBJ_96C @ 0x80087d98  (Ghidra headless, raw MIPS overlay) */

undefined4 BIOS_OBJ_96C(void)

{
  undefined1 uVar1;
  bool bVar2;
  uint uVar3;
  undefined4 uVar4;
  int iVar5;
  undefined1 *puVar6;
  byte unaff_s1;
  byte *unaff_s2;
  char *unaff_s3;
  undefined1 *unaff_s4;
  int unaff_s5;
  undefined1 *unaff_s6;
  int unaff_s7;
  
  do {
    while( true ) {
      uVar3 = getintr();
      if (uVar3 == 0) break;
      if (((uVar3 & 4) != 0) && (DAT_800acdf0 != (code *)0x0)) {
        (*DAT_800acdf0)(*unaff_s6,&DAT_800c3d48);
      }
      if (((uVar3 & 2) != 0) && (DAT_800acdec != (code *)0x0)) {
        (*DAT_800acdec)(*unaff_s2,&DAT_800c3d40);
      }
    }
    *PTR_CDROM_REG0_800ad0b4 = unaff_s1;
    do {
      if (*unaff_s3 != '\0') {
        *unaff_s3 = '\0';
        puVar6 = &DAT_800c3d50;
        if (unaff_s4 != (undefined1 *)0x0) {
          iVar5 = 7;
          do {
            uVar1 = *puVar6;
            puVar6 = puVar6 + 1;
            iVar5 = iVar5 + -1;
            *unaff_s4 = uVar1;
            unaff_s4 = unaff_s4 + 1;
          } while (iVar5 != -1);
          uVar4 = BIOS_OBJ_AA4();
          return uVar4;
        }
        goto BIOS_OBJ_A94;
      }
      if (unaff_s3[-1] != '\0') {
        unaff_s3[-1] = '\0';
        puVar6 = &DAT_800c3d48;
        iVar5 = 7;
        if (unaff_s4 != (undefined1 *)0x0) {
          do {
            uVar1 = *puVar6;
            puVar6 = puVar6 + 1;
            iVar5 = iVar5 + -1;
            *unaff_s4 = uVar1;
            unaff_s4 = unaff_s4 + 1;
          } while (iVar5 != -1);
        }
BIOS_OBJ_A94:
        uVar4 = BIOS_OBJ_AA4();
        return uVar4;
      }
      if (unaff_s7 != 0) {
        return 0;
      }
      iVar5 = VSync(-1);
      if ((DAT_800c3d58 < iVar5) ||
         (iVar5 = DAT_800c3d5c + 1, bVar2 = 0x3c0000 < DAT_800c3d5c, DAT_800c3d5c = iVar5, bVar2)) {
        FUN_8009881c("CD timeout: ");
        printf("%s:(%s) Sync=%s, Ready=%s\n",DAT_800c3d60,(&PTR_s_CdlSync_800ace14)[DAT_800ace0d],
               *(undefined4 *)((uint)*unaff_s2 * 4 + unaff_s5));
        CD_flush();
        uVar4 = BIOS_OBJ_93C();
        return uVar4;
      }
      iVar5 = CheckCallback();
    } while (iVar5 == 0);
    unaff_s1 = *PTR_CDROM_REG0_800ad0b4 & 3;
  } while( true );
}


