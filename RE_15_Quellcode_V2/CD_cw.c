int CD_cw(byte param_1,byte *param_2,undefined1 *param_3,int param_4)

{
  undefined1 uVar1;
  bool bVar2;
  int iVar3;
  uint uVar4;
  byte *pbVar5;
  undefined1 *puVar6;
  byte bVar7;
  
  if (1 < DAT_800788b0) {
    printf("%s...\n",(&PTR_s_CdlSync_800788c8)[param_1]);
  }
  if ((*(int *)(&DAT_80078ae8 + (uint)param_1 * 4) == 0) || (param_2 != (byte *)0x0)) {
    CD_sync(0,0);
    iVar3 = 0;
    pbVar5 = param_2;
    if (param_1 == 2) {
      do {
        (&DAT_800788c0)[iVar3] = *pbVar5;
        iVar3 = iVar3 + 1;
        pbVar5 = pbVar5 + 1;
      } while (iVar3 < 4);
    }
    DAT_80078b80 = 0;
    if (*(int *)(&DAT_800789e8 + (uint)param_1 * 4) != 0) {
      DAT_80078b81 = 0;
    }
    CDROM_REG0 = 0;
    iVar3 = 0;
    pbVar5 = param_2;
    if (0 < *(int *)(&DAT_80078ae8 + (uint)param_1 * 4)) {
      do {
        CDROM_REG2 = *pbVar5;
        iVar3 = iVar3 + 1;
        pbVar5 = pbVar5 + 1;
      } while (iVar3 < *(int *)(&DAT_80078ae8 + (uint)param_1 * 4));
    }
    iVar3 = 0;
    DAT_800788c5 = param_1;
    CDROM_REG1 = param_1;
    if (param_4 == 0) {
      iVar3 = FUN_80061fc0(0xffffffff);
      DAT_8008fe10 = iVar3 + 0x3c0;
      DAT_8008fe14 = 0;
      DAT_8008fe18 = "CD_cw";
      bVar7 = CDROM_REG0;
      while (CDROM_REG0 = bVar7, DAT_80078b80 == 0) {
        iVar3 = FUN_80061fc0(0xffffffff);
        if ((DAT_8008fe10 < iVar3) ||
           (iVar3 = DAT_8008fe14 + 1, bVar2 = 0x3c0000 < DAT_8008fe14, DAT_8008fe14 = iVar3, bVar2))
        {
          puts("CD timeout: ");
          printf("%s:(%s) Sync=%s, Ready=%s\n",DAT_8008fe18,(&PTR_s_CdlSync_800788c8)[DAT_800788c5],
                 (&PTR_s_NoIntr_80078948)[DAT_80078b80],(&PTR_s_NoIntr_80078948)[DAT_80078b81]);
          CD_flush();
          iVar3 = BIOS_OBJ_D88();
          return iVar3;
        }
        iVar3 = CheckCallback();
        bVar7 = CDROM_REG0;
        if (iVar3 != 0) {
          bVar7 = CDROM_REG0 & 3;
          while( true ) {
            uVar4 = getintr();
            if (uVar4 == 0) break;
            if (((uVar4 & 4) != 0) && (DAT_800788a8 != (code *)0x0)) {
              (*DAT_800788a8)(DAT_80078b81,&DAT_8008fe00);
            }
            if (((uVar4 & 2) != 0) && (DAT_800788a4 != (code *)0x0)) {
              (*DAT_800788a4)(DAT_80078b80,&DAT_8008fdf8);
            }
          }
        }
      }
      if ((DAT_80078b80 == 2) && (param_1 == 0xe)) {
        DAT_800788c4 = *param_2;
      }
      puVar6 = &DAT_8008fdf8;
      if (param_3 != (undefined1 *)0x0) {
        iVar3 = 7;
        do {
          uVar1 = *puVar6;
          puVar6 = puVar6 + 1;
          iVar3 = iVar3 + -1;
          *param_3 = uVar1;
          param_3 = param_3 + 1;
        } while (iVar3 != -1);
      }
      iVar3 = -(uint)(DAT_80078b80 == 5);
    }
  }
  else {
    iVar3 = -2;
    if (0 < DAT_800788b0) {
      printf("%s: no param\n",(&PTR_s_CdlSync_800788c8)[param_1]);
      iVar3 = BIOS_OBJ_EDC();
      return iVar3;
    }
  }
  return iVar3;
}
