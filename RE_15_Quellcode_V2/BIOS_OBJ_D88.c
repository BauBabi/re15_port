int BIOS_OBJ_D88(void)

{
  undefined1 uVar1;
  bool bVar2;
  int in_v0;
  int iVar3;
  uint uVar4;
  undefined1 *puVar5;
  byte bVar6;
  byte *unaff_s2;
  char unaff_s3;
  int unaff_s4;
  undefined1 *unaff_s5;
  undefined1 *unaff_s6;
  undefined1 *unaff_s7;
  
  while( true ) {
    if (in_v0 != 0) {
      return -1;
    }
    iVar3 = CheckCallback();
    bVar6 = CDROM_REG0;
    if (iVar3 != 0) {
      bVar6 = CDROM_REG0 & 3;
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
    CDROM_REG0 = bVar6;
    if (*unaff_s2 != 0) break;
    iVar3 = FUN_80061fc0(0xffffffff);
    if ((DAT_8008fe10 < iVar3) ||
       (iVar3 = DAT_8008fe14 + 1, bVar2 = 0x3c0000 < DAT_8008fe14, DAT_8008fe14 = iVar3, bVar2)) {
      puts("CD timeout: ");
      printf("%s:(%s) Sync=%s, Ready=%s\n",DAT_8008fe18,(&PTR_s_CdlSync_800788c8)[DAT_800788c5],
             *(undefined4 *)((uint)*unaff_s2 * 4 + unaff_s4));
      CD_flush();
      iVar3 = BIOS_OBJ_D88();
      return iVar3;
    }
    in_v0 = 0;
  }
  if ((DAT_80078b80 == '\x02') && (unaff_s3 == '\x0e')) {
    DAT_800788c4 = *unaff_s5;
  }
  puVar5 = &DAT_8008fdf8;
  if (unaff_s7 != (undefined1 *)0x0) {
    iVar3 = 7;
    do {
      uVar1 = *puVar5;
      puVar5 = puVar5 + 1;
      iVar3 = iVar3 + -1;
      *unaff_s7 = uVar1;
      unaff_s7 = unaff_s7 + 1;
    } while (iVar3 != -1);
  }
  return -(uint)(DAT_80078b80 == '\x05');
}
