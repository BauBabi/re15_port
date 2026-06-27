int BIOS_OBJ_DB8(void)

{
  undefined1 uVar1;
  bool bVar2;
  byte bVar3;
  uint uVar4;
  int iVar5;
  undefined1 *puVar6;
  byte unaff_s1;
  byte *unaff_s2;
  char unaff_s3;
  int unaff_s4;
  undefined1 *unaff_s5;
  undefined1 *unaff_s6;
  undefined1 *unaff_s7;
  
  do {
    while( true ) {
      uVar4 = getintr();
      bVar3 = unaff_s1;
      if (uVar4 == 0) break;
      if (((uVar4 & 4) != 0) && (DAT_800788a8 != (code *)0x0)) {
        (*DAT_800788a8)(*unaff_s6,&DAT_8008fe00);
      }
      if (((uVar4 & 2) != 0) && (DAT_800788a4 != (code *)0x0)) {
        (*DAT_800788a4)(*unaff_s2,&DAT_8008fdf8);
      }
    }
    do {
      CDROM_REG0 = bVar3;
      if (*unaff_s2 != 0) {
        if ((DAT_80078b80 == '\x02') && (unaff_s3 == '\x0e')) {
          DAT_800788c4 = *unaff_s5;
        }
        puVar6 = &DAT_8008fdf8;
        if (unaff_s7 != (undefined1 *)0x0) {
          iVar5 = 7;
          do {
            uVar1 = *puVar6;
            puVar6 = puVar6 + 1;
            iVar5 = iVar5 + -1;
            *unaff_s7 = uVar1;
            unaff_s7 = unaff_s7 + 1;
          } while (iVar5 != -1);
        }
        return -(uint)(DAT_80078b80 == '\x05');
      }
      iVar5 = FUN_80061fc0(0xffffffff);
      if ((DAT_8008fe10 < iVar5) ||
         (iVar5 = DAT_8008fe14 + 1, bVar2 = 0x3c0000 < DAT_8008fe14, DAT_8008fe14 = iVar5, bVar2)) {
        puts("CD timeout: ");
        printf("%s:(%s) Sync=%s, Ready=%s\n",DAT_8008fe18,(&PTR_s_CdlSync_800788c8)[DAT_800788c5],
               *(undefined4 *)((uint)*unaff_s2 * 4 + unaff_s4));
        CD_flush();
        iVar5 = BIOS_OBJ_D88();
        return iVar5;
      }
      iVar5 = CheckCallback();
      bVar3 = CDROM_REG0;
    } while (iVar5 == 0);
    unaff_s1 = CDROM_REG0 & 3;
  } while( true );
}
