undefined4 CD_sync(int param_1,undefined1 *param_2)

{
  undefined1 uVar1;
  bool bVar2;
  int iVar3;
  undefined4 uVar4;
  uint uVar5;
  undefined1 *puVar6;
  byte bVar7;
  
  iVar3 = FUN_80061fc0(0xffffffff);
  DAT_8008fe10 = iVar3 + 0x3c0;
  DAT_8008fe14 = 0;
  DAT_8008fe18 = "CD_sync";
  while( true ) {
    iVar3 = FUN_80061fc0(0xffffffff);
    if ((DAT_8008fe10 < iVar3) ||
       (iVar3 = DAT_8008fe14 + 1, bVar2 = 0x3c0000 < DAT_8008fe14, DAT_8008fe14 = iVar3, bVar2)) {
      puts("CD timeout: ");
      printf("%s:(%s) Sync=%s, Ready=%s\n",DAT_8008fe18,(&PTR_s_CdlSync_800788c8)[DAT_800788c5],
             (&PTR_s_NoIntr_80078948)[DAT_80078b80],(&PTR_s_NoIntr_80078948)[DAT_80078b81]);
      CD_flush();
      uVar4 = BIOS_OBJ_6A4();
      return uVar4;
    }
    iVar3 = CheckCallback();
    bVar7 = CDROM_REG0;
    if (iVar3 != 0) {
      bVar7 = CDROM_REG0 & 3;
      while( true ) {
        uVar5 = getintr();
        if (uVar5 == 0) break;
        if (((uVar5 & 4) != 0) && (DAT_800788a8 != (code *)0x0)) {
          (*DAT_800788a8)(DAT_80078b81,&DAT_8008fe00);
        }
        if (((uVar5 & 2) != 0) && (DAT_800788a4 != (code *)0x0)) {
          (*DAT_800788a4)(DAT_80078b80,&DAT_8008fdf8);
        }
      }
    }
    CDROM_REG0 = bVar7;
    if ((DAT_80078b80 == 2) || (DAT_80078b80 == 5)) break;
    if (param_1 != 0) {
      return 0;
    }
  }
  DAT_80078b80 = 2;
  puVar6 = &DAT_8008fdf8;
  iVar3 = 7;
  if (param_2 != (undefined1 *)0x0) {
    do {
      uVar1 = *puVar6;
      puVar6 = puVar6 + 1;
      iVar3 = iVar3 + -1;
      *param_2 = uVar1;
      param_2 = param_2 + 1;
    } while (iVar3 != -1);
  }
  uVar4 = BIOS_OBJ_7CC();
  return uVar4;
}
