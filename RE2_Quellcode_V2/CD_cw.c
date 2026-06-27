/* CD_cw @ 0x80087f00  (Ghidra headless, raw MIPS overlay) */

int CD_cw(byte param_1,undefined1 *param_2,undefined1 *param_3,int param_4)

{
  undefined1 uVar1;
  byte bVar2;
  bool bVar3;
  int iVar4;
  uint uVar5;
  undefined1 *puVar6;
  int iVar7;
  
  if (1 < DAT_800acdf8) {
    printf("%s...\n",(&PTR_s_CdlSync_800ace14)[param_1]);
  }
  if ((*(int *)(&DAT_800ad034 + (uint)param_1 * 4) == 0) || (param_2 != (undefined1 *)0x0)) {
    CD_sync(0,0);
    if (param_1 == 2) {
      iVar4 = 0;
      puVar6 = param_2;
      do {
        (&DAT_800ace08)[iVar4] = *puVar6;
        iVar4 = iVar4 + 1;
        puVar6 = puVar6 + 1;
      } while (iVar4 < 4);
    }
    if (param_1 == 0xe) {
      DAT_800ace0c = *param_2;
    }
    iVar4 = (uint)param_1 * 4;
    DAT_800ad0cc = 0;
    if (*(int *)(&DAT_800acf34 + iVar4) != 0) {
      DAT_800ad0cd = 0;
    }
    *PTR_CDROM_REG0_800ad0b4 = 0;
    iVar7 = 0;
    if (0 < *(int *)(&DAT_800ad034 + iVar4)) {
      do {
        uVar1 = *param_2;
        param_2 = param_2 + 1;
        *PTR_CDROM_REG2_800ad0bc = uVar1;
        iVar7 = iVar7 + 1;
      } while (iVar7 < *(int *)(&DAT_800ad034 + iVar4));
    }
    DAT_800ace0d = param_1;
    *PTR_CDROM_REG1_800ad0b8 = param_1;
    iVar4 = 0;
    if (param_4 == 0) {
      iVar4 = VSync(-1);
      DAT_800c3d58 = iVar4 + 0x3c0;
      DAT_800c3d5c = 0;
      DAT_800c3d60 = "CD_cw";
      while (DAT_800ad0cc == 0) {
        iVar4 = VSync(-1);
        if ((DAT_800c3d58 < iVar4) ||
           (iVar4 = DAT_800c3d5c + 1, bVar3 = 0x3c0000 < DAT_800c3d5c, DAT_800c3d5c = iVar4, bVar3))
        {
          FUN_8009881c("CD timeout: ");
          printf("%s:(%s) Sync=%s, Ready=%s\n",DAT_800c3d60,(&PTR_s_CdlSync_800ace14)[DAT_800ace0d],
                 (&PTR_s_NoIntr_800ace94)[DAT_800ad0cc],(&PTR_s_NoIntr_800ace94)[DAT_800ad0cd]);
          CD_flush();
          iVar4 = BIOS_OBJ_DAC();
          return iVar4;
        }
        iVar4 = CheckCallback();
        if (iVar4 != 0) {
          bVar2 = *PTR_CDROM_REG0_800ad0b4;
          while( true ) {
            uVar5 = getintr();
            if (uVar5 == 0) break;
            if (((uVar5 & 4) != 0) && (DAT_800acdf0 != (code *)0x0)) {
              (*DAT_800acdf0)(DAT_800ad0cd,&DAT_800c3d48);
            }
            if (((uVar5 & 2) != 0) && (DAT_800acdec != (code *)0x0)) {
              (*DAT_800acdec)(DAT_800ad0cc,&DAT_800c3d40);
            }
          }
          *PTR_CDROM_REG0_800ad0b4 = bVar2 & 3;
        }
      }
      puVar6 = &DAT_800c3d40;
      if (param_3 != (undefined1 *)0x0) {
        iVar4 = 7;
        do {
          uVar1 = *puVar6;
          puVar6 = puVar6 + 1;
          iVar4 = iVar4 + -1;
          *param_3 = uVar1;
          param_3 = param_3 + 1;
        } while (iVar4 != -1);
      }
      iVar4 = -(uint)(DAT_800ad0cc == 5);
    }
  }
  else {
    iVar4 = -2;
    if (0 < DAT_800acdf8) {
      printf("%s: no param\n",(&PTR_s_CdlSync_800ace14)[param_1]);
      iVar4 = BIOS_OBJ_EC8();
      return iVar4;
    }
  }
  return iVar4;
}


