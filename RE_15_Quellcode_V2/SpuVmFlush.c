void SpuVmFlush(void)

{
  short sVar1;
  undefined2 *puVar2;
  uint *puVar3;
  uint uVar4;
  short *psVar5;
  int iVar6;
  dword *pdVar7;
  byte *pbVar8;
  undefined2 *puVar9;
  undefined2 *puVar10;
  undefined2 *puVar11;
  uint uVar12;
  int iVar13;
  int iVar14;
  int iVar15;
  
  uVar12 = 0;
  uVar4 = (uint)DAT_800b52a8;
  DAT_8008fd24 = DAT_8008fd24 + 1 & 0xf;
  puVar3 = &DAT_8008fd28 + DAT_8008fd24;
  *puVar3 = 0;
  if (uVar4 != 0) {
    psVar5 = &DAT_8008f84a;
    pdVar7 = &VOICE_00_LEFT_RIGHT;
    do {
      *psVar5 = (short)pdVar7[3];
      sVar1 = *psVar5;
      psVar5 = psVar5 + 0x1a;
      if (sVar1 == 0) {
        *puVar3 = 1 << (uVar12 & 0x1f) | *puVar3;
      }
      uVar12 = uVar12 + 1;
      pdVar7 = pdVar7 + 4;
    } while ((int)uVar12 < (int)uVar4);
  }
  iVar13 = 0;
  if (DAT_800b535c == '\0') {
    uVar4 = 0xffffffff;
    puVar3 = &DAT_8008fd28;
    do {
      iVar13 = iVar13 + 1;
      uVar4 = uVar4 & *puVar3;
      puVar3 = puVar3 + 1;
    } while (iVar13 < 0xf);
    uVar12 = 0;
    if (DAT_800b52a8 != 0) {
      iVar13 = 0;
      do {
        if ((uVar4 & 1 << (uVar12 & 0x1f)) != 0) {
          if ((&DAT_8008f85f)[iVar13] == '\x02') {
            SpuSetNoiseVoice(0,0xffffff);
          }
          (&DAT_8008f85f)[iVar13] = 0;
        }
        uVar12 = uVar12 + 1;
        iVar13 = iVar13 + 0x34;
      } while ((int)uVar12 < (int)(uint)DAT_800b52a8);
    }
  }
  iVar13 = 0;
  iVar15 = 0;
  DAT_8008ff84 = DAT_8008ff84 & ~DAT_800bee78;
  iVar14 = 0;
  DAT_8008ff88 = DAT_8008ff88 & ~DAT_800bee80;
  do {
    if (*(short *)((int)&DAT_8008f860 + iVar14) != 0) {
      SetAutoVol(iVar15 >> 0x10);
    }
    psVar5 = (short *)((int)&DAT_8008f86c + iVar14);
    iVar14 = iVar14 + 0x34;
    if (*psVar5 != 0) {
      SetAutoPan(iVar15 >> 0x10);
    }
    iVar15 = iVar15 + 0x10000;
    iVar13 = iVar13 + 1;
    iVar6 = 0;
  } while (iVar13 < 0x18);
  pbVar8 = &DAT_8008f82c;
  puVar2 = &DAT_8008f6ac;
  puVar11 = &DAT_8008f6b6;
  puVar10 = &DAT_8008f6b4;
  puVar9 = &DAT_8008f6ae;
  do {
    if ((*pbVar8 & 1) != 0) {
      *(undefined2 *)((int)&VOICE_00_LEFT_RIGHT + iVar6) = *puVar2;
      *(undefined2 *)((int)&VOICE_00_LEFT_RIGHT + iVar6 + 2) = *puVar9;
    }
    if ((*pbVar8 & 4) != 0) {
      *(undefined2 *)((int)&VOICE_00_ADPCM_SAMPLE_RATE + iVar6) =
           *(undefined2 *)((int)&DAT_8008f6b0 + iVar6);
    }
    if ((*pbVar8 & 8) != 0) {
      *(undefined2 *)((int)&VOICE_00_ADPCM_START_ADDR + iVar6) =
           *(undefined2 *)((int)&DAT_8008f6b2 + iVar6);
    }
    puVar9 = puVar9 + 8;
    if ((*pbVar8 & 0x10) != 0) {
      *(undefined2 *)((int)&VOICE_00_ADSR_ATT_DEC_SUS_REL + iVar6) = *puVar10;
      *(undefined2 *)((int)&DAT_1f801c0a + iVar6) = *puVar11;
    }
    *pbVar8 = 0;
    SPU_VOICE_KEY_OFF._2_2_ = DAT_800bee80;
    SPU_VOICE_KEY_OFF._0_2_ = DAT_800bee78;
    SPU_VOICE_KEY_ON._2_2_ = DAT_8008ff88;
    SPU_VOICE_KEY_ON._0_2_ = DAT_8008ff84;
    pbVar8 = pbVar8 + 1;
    puVar11 = puVar11 + 8;
    puVar10 = puVar10 + 8;
    iVar6 = iVar6 + 0x10;
    puVar2 = puVar2 + 8;
  } while ((int)pbVar8 < -0x7ff707bc);
  DAT_800bee78 = 0;
  DAT_800bee80 = 0;
  DAT_8008ff84 = 0;
  DAT_8008ff88 = 0;
  SPU_VOICE_CHN_REVERB_MODE._0_2_ = DAT_8008ff8c;
  SPU_VOICE_CHN_REVERB_MODE._2_2_ = DAT_8008ff90;
  return;
}
