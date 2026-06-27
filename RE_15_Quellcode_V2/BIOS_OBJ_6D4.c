undefined4 BIOS_OBJ_6D4(void)

{
  undefined1 uVar1;
  bool bVar2;
  byte bVar3;
  uint uVar4;
  undefined4 uVar5;
  int iVar6;
  undefined1 *puVar7;
  byte unaff_s1;
  byte *unaff_s2;
  int unaff_s3;
  undefined1 *unaff_s4;
  int unaff_s5;
  undefined1 *unaff_s6;
  
  do {
    while( true ) {
      uVar4 = getintr();
      bVar3 = unaff_s1;
      if (uVar4 == 0) break;
      if (((uVar4 & 4) != 0) && (DAT_800788a8 != (code *)0x0)) {
        (*DAT_800788a8)(*unaff_s4,&DAT_8008fe00);
      }
      if (((uVar4 & 2) != 0) && (DAT_800788a4 != (code *)0x0)) {
        (*DAT_800788a4)(*unaff_s2,&DAT_8008fdf8);
      }
    }
    do {
      CDROM_REG0 = bVar3;
      if ((*unaff_s2 == 2) || (*unaff_s2 == 5)) {
        *unaff_s2 = 2;
        puVar7 = &DAT_8008fdf8;
        iVar6 = 7;
        if (unaff_s6 != (undefined1 *)0x0) {
          do {
            uVar1 = *puVar7;
            puVar7 = puVar7 + 1;
            iVar6 = iVar6 + -1;
            *unaff_s6 = uVar1;
            unaff_s6 = unaff_s6 + 1;
          } while (iVar6 != -1);
        }
        uVar5 = BIOS_OBJ_7CC();
        return uVar5;
      }
      if (unaff_s5 != 0) {
        return 0;
      }
      iVar6 = FUN_80061fc0(0xffffffff);
      if ((DAT_8008fe10 < iVar6) ||
         (iVar6 = DAT_8008fe14 + 1, bVar2 = 0x3c0000 < DAT_8008fe14, DAT_8008fe14 = iVar6, bVar2)) {
        puts("CD timeout: ");
        printf("%s:(%s) Sync=%s, Ready=%s\n",DAT_8008fe18,(&PTR_s_CdlSync_800788c8)[DAT_800788c5],
               *(undefined4 *)((uint)*unaff_s2 * 4 + unaff_s3));
        CD_flush();
        uVar5 = BIOS_OBJ_6A4();
        return uVar5;
      }
      iVar6 = CheckCallback();
      bVar3 = CDROM_REG0;
    } while (iVar6 == 0);
    unaff_s1 = CDROM_REG0 & 3;
  } while( true );
}
