/* FUN_800754ac @ 0x800754ac  (Ghidra headless, raw MIPS overlay) */

void FUN_800754ac(int param_1)

{
  char cVar1;
  u_short uVar2;
  short sVar3;
  uint uVar4;
  short *psVar5;
  int iVar6;
  int iVar7;
  undefined *p;
  undefined *ot;
  undefined *p_00;
  undefined *puVar8;
  
  p_00 = &UNK_80198100;
  p = &UNK_80198180;
  iVar6 = 0x20;
  puVar8 = (&PTR_DAT_800ab180)[param_1];
  ot = &UNK_800cc1f0 + (uint)DAT_800ce5e0 * 0x20;
  if (DAT_800ce5e0 != 0) {
    p = &DAT_80198190;
    p_00 = &DAT_80198114;
  }
  DAT_800d5c50 = DAT_800d5c50 + 1 & 0x7f;
  if (DAT_800d5c52 == '\x01') {
    if (DAT_800d5c53 == '\0') {
      psVar5 = (short *)(p + 0xe);
      DAT_800d5c54 = DAT_800d5c54 + 3;
      do {
        iVar7 = (int)DAT_800d5c54;
        iVar6 = iVar6 + -1;
        if ((iVar7 - iVar6) - 0xbU < 0x52) {
          *(undefined1 *)(psVar5 + -5) = 0x10;
          *(char *)((int)psVar5 + -9) = ~((char)iVar6 * '\b');
          *(undefined1 *)(psVar5 + -4) = 0x10;
          sVar3 = DAT_800d5c2c + (short)(iVar7 - iVar6);
          psVar5[-1] = sVar3;
          psVar5[-3] = sVar3;
          psVar5[-2] = DAT_800d5c2e + 4;
          *psVar5 = DAT_800d5c2e + 0x23;
          AddPrim(ot,p);
        }
        psVar5 = psVar5 + 0x10;
        p = p + 0x20;
      } while (iVar6 != 0);
LAB_800758d0:
      if (DAT_800d5c54 < 0x81) goto LAB_800758e8;
LAB_800758e4:
      DAT_800d5c53 = '\x01';
      goto LAB_800758e8;
    }
LAB_80075818:
    DAT_800d5c54 = 10;
  }
  else {
    if (DAT_800d5c52 < '\x02') {
      if (DAT_800d5c52 == '\0') {
        iVar7 = param_1 * 6;
        psVar5 = (short *)(p + 0xe);
        do {
          iVar6 = iVar6 + -1;
          uVar4 = (short)DAT_800d5c50 - iVar6;
          if (uVar4 < 0x50) {
            cVar1 = (char)iVar6;
            *(char *)(psVar5 + -5) = (&DAT_800ab160)[iVar7] - (&DAT_800ab163)[iVar7] * cVar1;
            *(char *)((int)psVar5 + -9) = (&DAT_800ab161)[iVar7] - (&DAT_800ab164)[iVar7] * cVar1;
            *(char *)(psVar5 + -4) = (&DAT_800ab162)[iVar7] - (&DAT_800ab165)[iVar7] * cVar1;
            sVar3 = DAT_800d5c2c + (short)uVar4 + 0xc;
            psVar5[-3] = sVar3;
            psVar5[-1] = sVar3;
            sVar3 = DAT_800d5c2e + (byte)puVar8[uVar4 * 2] + 2;
            psVar5[-2] = sVar3;
            *psVar5 = sVar3 + (ushort)(byte)(puVar8 + uVar4 * 2)[1];
            AddPrim(ot,p);
          }
          psVar5 = psVar5 + 0x10;
          p = p + 0x20;
        } while (iVar6 != 0);
      }
      goto LAB_800758e8;
    }
    if (DAT_800d5c52 != '\x02') {
      if (DAT_800d5c52 != '\x03') goto LAB_800758e8;
      if (DAT_800d5c53 == '\0') {
        psVar5 = (short *)(p + 0xe);
        DAT_800d5c54 = DAT_800d5c54 + 3;
        do {
          iVar7 = (int)DAT_800d5c54;
          iVar6 = iVar6 + -1;
          if ((iVar7 - iVar6) - 0xbU < 0x52) {
            cVar1 = (char)iVar6 * -8 + -1;
            *(char *)(psVar5 + -5) = cVar1;
            *(char *)((int)psVar5 + -9) = cVar1;
            *(undefined1 *)(psVar5 + -4) = 0x10;
            sVar3 = DAT_800d5c2c + (short)(iVar7 - iVar6);
            psVar5[-1] = sVar3;
            psVar5[-3] = sVar3;
            psVar5[-2] = DAT_800d5c2e + 4;
            *psVar5 = DAT_800d5c2e + 0x23;
            AddPrim(ot,p);
          }
          psVar5 = psVar5 + 0x10;
          p = p + 0x20;
        } while (iVar6 != 0);
        goto LAB_800758d0;
      }
      goto LAB_80075818;
    }
    if (DAT_800d5c53 == '\0') {
      psVar5 = (short *)(p + 10);
      DAT_800d5c56 = DAT_800d5c56 + -3;
      do {
        iVar7 = (int)DAT_800d5c56;
        iVar6 = iVar6 + -1;
        if ((iVar7 - iVar6) - 3U < 0x21) {
          *(undefined1 *)((int)psVar5 + -5) = 0x10;
          *(undefined1 *)(psVar5 + -3) = 0x10;
          *(char *)(psVar5 + -2) = (char)iVar6 * '\b';
          psVar5[-1] = DAT_800d5c2c + 0xb;
          psVar5[1] = DAT_800d5c2c + 0x5c;
          sVar3 = DAT_800d5c2e + (short)(iVar7 - iVar6);
          psVar5[2] = sVar3;
          *psVar5 = sVar3;
          AddPrim(ot,p);
        }
        psVar5 = psVar5 + 0x10;
        p = p + 0x20;
      } while (iVar6 != 0);
      if (-0x15 < DAT_800d5c56) goto LAB_800758e8;
      goto LAB_800758e4;
    }
    DAT_800d5c56 = 0x46;
  }
  DAT_800d5c52 = '\0';
  DAT_800d5c53 = '\0';
  DAT_800d5c50 = 0;
LAB_800758e8:
  AddPrim(ot,&UNK_800d6bf0 + (uint)DAT_800ce5e0 * 0xc);
  p_00[0xc] = (char)param_1 * '.';
  p_00[0xd] = 0x23;
  *(short *)(p_00 + 8) = DAT_800d5c2c + 0x2f;
  *(short *)(p_00 + 10) = DAT_800d5c2e + 0x19;
  uVar2 = GetClut(0x100,0x1eb);
  *(u_short *)(p_00 + 0xe) = uVar2;
  AddPrim(ot,p_00);
  AddPrim(ot,&UNK_800d6ae8 + (uint)DAT_800ce5e0 * 0xc);
  *(short *)(p_00 + 0x30) = DAT_800d5c2c + 2;
  *(short *)(p_00 + 0x32) = DAT_800d5c2e;
  AddPrim(ot,p_00 + 0x28);
  AddPrim(ot,&UNK_800d69f8 + (uint)DAT_800ce5e0 * 0xc);
  return;
}


