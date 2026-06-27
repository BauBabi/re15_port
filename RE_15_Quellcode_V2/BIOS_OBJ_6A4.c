undefined4 BIOS_OBJ_6A4(void)

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
  int unaff_s3;
  undefined1 *unaff_s4;
  int unaff_s5;
  undefined1 *unaff_s6;
  
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
          (*DAT_800788a8)(*unaff_s4,&DAT_8008fe00);
        }
        if (((uVar4 & 2) != 0) && (DAT_800788a4 != (code *)0x0)) {
          (*DAT_800788a4)(*unaff_s2,&DAT_8008fdf8);
        }
      }
    }
    CDROM_REG0 = bVar7;
    if ((*unaff_s2 == 2) || (*unaff_s2 == 5)) {
      *unaff_s2 = 2;
      puVar6 = &DAT_8008fdf8;
      iVar3 = 7;
      if (unaff_s6 != (undefined1 *)0x0) {
        do {
          uVar1 = *puVar6;
          puVar6 = puVar6 + 1;
          iVar3 = iVar3 + -1;
          *unaff_s6 = uVar1;
          unaff_s6 = unaff_s6 + 1;
        } while (iVar3 != -1);
      }
      uVar5 = BIOS_OBJ_7CC();
      return uVar5;
    }
    if (unaff_s5 != 0) {
      return 0;
    }
    iVar3 = FUN_80061fc0(0xffffffff);
    if ((DAT_8008fe10 < iVar3) ||
       (iVar3 = DAT_8008fe14 + 1, bVar2 = 0x3c0000 < DAT_8008fe14, DAT_8008fe14 = iVar3, bVar2))
    break;
    in_v0 = 0;
  }
  puts("CD timeout: ");
  printf("%s:(%s) Sync=%s, Ready=%s\n",DAT_8008fe18,(&PTR_s_CdlSync_800788c8)[DAT_800788c5],
         *(undefined4 *)((uint)*unaff_s2 * 4 + unaff_s3));
  CD_flush();
  uVar5 = BIOS_OBJ_6A4();
  return uVar5;
}
