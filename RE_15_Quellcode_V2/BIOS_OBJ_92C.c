undefined4 BIOS_OBJ_92C(void)

{
  undefined1 uVar1;
  bool bVar2;
  int in_v0;
  int iVar3;
  uint uVar4;
  undefined4 uVar5;
  undefined1 *puVar6;
  byte bVar7;
  byte *unaff_s2;
  char *unaff_s3;
  undefined1 *unaff_s4;
  int unaff_s5;
  undefined1 *unaff_s6;
  int unaff_s7;
  
  while( true ) {
    if (in_v0 != 0) {
      return 0xffffffff;
    }
    iVar3 = CheckCallback();
    bVar7 = CDROM_REG0;
    if (iVar3 != 0) {
      bVar7 = CDROM_REG0 & 3;
      while( true ) {
        uVar4 = getintr();
        if (uVar4 == 0) break;
        if (((uVar4 & 4) != 0) && (DAT_800788a8 != (code *)0x0)) {
          (*DAT_800788a8)(*unaff_s6,&DAT_8008fe00);
        }
        if (((uVar4 & 2) != 0) && (DAT_800788a4 != (code *)0x0)) {
          (*DAT_800788a4)(*unaff_s2,&DAT_8008fdf8);
        }
      }
    }
    CDROM_REG0 = bVar7;
    if (*unaff_s3 != '\0') break;
    if (unaff_s3[-1] != '\0') {
      unaff_s3[-1] = '\0';
      puVar6 = &DAT_8008fe00;
      iVar3 = 7;
      if (unaff_s4 != (undefined1 *)0x0) {
        do {
          uVar1 = *puVar6;
          puVar6 = puVar6 + 1;
          iVar3 = iVar3 + -1;
          *unaff_s4 = uVar1;
          unaff_s4 = unaff_s4 + 1;
        } while (iVar3 != -1);
      }
      goto BIOS_OBJ_A84;
    }
    if (unaff_s7 != 0) {
      return 0;
    }
    iVar3 = FUN_80061fc0(0xffffffff);
    if ((DAT_8008fe10 < iVar3) ||
       (iVar3 = DAT_8008fe14 + 1, bVar2 = 0x3c0000 < DAT_8008fe14, DAT_8008fe14 = iVar3, bVar2)) {
      puts("CD timeout: ");
      printf("%s:(%s) Sync=%s, Ready=%s\n",DAT_8008fe18,(&PTR_s_CdlSync_800788c8)[DAT_800788c5],
             *(undefined4 *)((uint)*unaff_s2 * 4 + unaff_s5));
      CD_flush();
      uVar5 = BIOS_OBJ_92C();
      return uVar5;
    }
    in_v0 = 0;
  }
  *unaff_s3 = '\0';
  puVar6 = &DAT_8008fe08;
  if (unaff_s4 != (undefined1 *)0x0) {
    iVar3 = 7;
    do {
      uVar1 = *puVar6;
      puVar6 = puVar6 + 1;
      iVar3 = iVar3 + -1;
      *unaff_s4 = uVar1;
      unaff_s4 = unaff_s4 + 1;
    } while (iVar3 != -1);
    uVar5 = BIOS_OBJ_A94();
    return uVar5;
  }
BIOS_OBJ_A84:
  uVar5 = BIOS_OBJ_A94();
  return uVar5;
}
