/* CD_sync @ 0x800879b4  (Ghidra headless, raw MIPS overlay) */

undefined4 CD_sync(int param_1,undefined1 *param_2)

{
  byte bVar1;
  undefined1 uVar2;
  bool bVar3;
  int iVar4;
  undefined4 uVar5;
  uint uVar6;
  undefined1 *puVar7;
  
  iVar4 = VSync(-1);
  DAT_800c3d58 = iVar4 + 0x3c0;
  DAT_800c3d5c = 0;
  DAT_800c3d60 = "CD_sync";
  while( true ) {
    iVar4 = VSync(-1);
    if ((DAT_800c3d58 < iVar4) ||
       (iVar4 = DAT_800c3d5c + 1, bVar3 = 0x3c0000 < DAT_800c3d5c, DAT_800c3d5c = iVar4, bVar3)) {
      FUN_8009881c("CD timeout: ");
      printf("%s:(%s) Sync=%s, Ready=%s\n",DAT_800c3d60,(&PTR_s_CdlSync_800ace14)[DAT_800ace0d],
             (&PTR_s_NoIntr_800ace94)[DAT_800ad0cc],(&PTR_s_NoIntr_800ace94)[DAT_800ad0cd]);
      CD_flush();
      uVar5 = BIOS_OBJ_6B4();
      return uVar5;
    }
    iVar4 = CheckCallback();
    if (iVar4 != 0) {
      bVar1 = *PTR_CDROM_REG0_800ad0b4;
      while( true ) {
        uVar6 = getintr();
        if (uVar6 == 0) break;
        if (((uVar6 & 4) != 0) && (DAT_800acdf0 != (code *)0x0)) {
          (*DAT_800acdf0)(DAT_800ad0cd,&DAT_800c3d48);
        }
        if (((uVar6 & 2) != 0) && (DAT_800acdec != (code *)0x0)) {
          (*DAT_800acdec)(DAT_800ad0cc,&DAT_800c3d40);
        }
      }
      *PTR_CDROM_REG0_800ad0b4 = bVar1 & 3;
    }
    if ((DAT_800ad0cc == 2) || (DAT_800ad0cc == 5)) break;
    if (param_1 != 0) {
      return 0;
    }
  }
  DAT_800ad0cc = 2;
  puVar7 = &DAT_800c3d40;
  iVar4 = 7;
  if (param_2 != (undefined1 *)0x0) {
    do {
      uVar2 = *puVar7;
      puVar7 = puVar7 + 1;
      iVar4 = iVar4 + -1;
      *param_2 = uVar2;
      param_2 = param_2 + 1;
    } while (iVar4 != -1);
  }
  uVar5 = BIOS_OBJ_7DC();
  return uVar5;
}


