/* FUN_80081f20 @ 0x80081f20  (Ghidra headless, raw MIPS overlay) */

void FUN_80081f20(void)

{
  undefined2 *puVar1;
  uint *puVar2;
  int iVar3;
  byte *pbVar4;
  uint uVar5;
  uint uVar6;
  int iVar7;
  short *envx;
  undefined2 *puVar8;
  undefined2 *puVar9;
  undefined2 *puVar10;
  int local_78;
  uint local_74;
  undefined2 local_70;
  undefined2 local_6e;
  undefined2 local_64;
  int local_5c;
  undefined2 local_3e;
  undefined2 local_3c;
  
  DAT_800e8764 = DAT_800e8764 + 1 & 0xf;
  *(undefined4 *)(&DAT_800eae58 + DAT_800e8764 * 4) = 0;
  uVar5 = 0;
  if (DAT_800d7854 != 0) {
    envx = &DAT_800cbcd6;
    iVar3 = 0;
    do {
      SpuGetVoiceEnvelope(uVar5,envx);
      if (*(short *)((int)&DAT_800cbcd6 + iVar3) == 0) {
        *(uint *)(&DAT_800eae58 + DAT_800e8764 * 4) =
             *(uint *)(&DAT_800eae58 + DAT_800e8764 * 4) | 1 << (uVar5 & 0x1f);
      }
      envx = envx + 0x1b;
      uVar5 = uVar5 + 1;
      iVar3 = iVar3 + 0x36;
    } while ((int)uVar5 < (int)(uint)DAT_800d7854);
  }
  iVar3 = 0;
  if (DAT_800dfade == '\0') {
    uVar5 = 0xffffffff;
    puVar2 = (uint *)&DAT_800eae58;
    do {
      iVar3 = iVar3 + 1;
      uVar5 = uVar5 & *puVar2;
      puVar2 = puVar2 + 1;
    } while (iVar3 < 0xf);
    uVar6 = 0;
    if (DAT_800d7854 != 0) {
      iVar3 = 0;
      do {
        if ((uVar5 & 1 << (uVar6 & 0x1f)) != 0) {
          if ((&DAT_800cbced)[iVar3] == '\x02') {
            SpuSetNoiseVoice(0,0xffffff);
          }
          (&DAT_800cbced)[iVar3] = 0;
        }
        uVar6 = uVar6 + 1;
        iVar3 = iVar3 + 0x36;
      } while ((int)uVar6 < (int)(uint)DAT_800d7854);
    }
  }
  iVar7 = 0;
  DAT_800c440c = DAT_800c440c & ~DAT_800eaea0;
  iVar3 = 0;
  DAT_800c440e = DAT_800c440e & ~DAT_800eaea2;
  do {
    if (*(short *)((int)&DAT_800cbcee + iVar3) != 0) {
      (*DAT_800df330)(iVar7);
    }
    if (*(short *)((int)&DAT_800cbcfa + iVar3) != 0) {
      (*DAT_800dcbd8)(iVar7);
    }
    iVar7 = iVar7 + 1;
    iVar3 = iVar3 + 0x36;
  } while (iVar7 < 0x18);
  uVar5 = 0;
  pbVar4 = &DAT_800d4c80;
  puVar1 = &DAT_800dccb0;
  puVar10 = &DAT_800dccba;
  puVar9 = &DAT_800dccb8;
  iVar3 = 0;
  puVar8 = &DAT_800dccb2;
  do {
    local_78 = 1 << (uVar5 & 0x1f);
    local_74 = 0;
    if ((*pbVar4 & 1) != 0) {
      local_74 = 3;
      local_70 = *puVar1;
      local_6e = *puVar8;
    }
    if ((*pbVar4 & 4) != 0) {
      local_74 = local_74 | 0x10;
      local_64 = *(undefined2 *)((int)&DAT_800dccb4 + iVar3);
    }
    if ((*pbVar4 & 8) != 0) {
      local_74 = local_74 | 0x80;
      local_5c = (uint)*(ushort *)((int)&DAT_800dccb6 + iVar3) << 3;
    }
    if ((*pbVar4 & 0x10) != 0) {
      local_74 = local_74 | 0x60000;
      local_3e = *puVar9;
      local_3c = *puVar10;
    }
    if (local_74 != 0) {
      FUN_8007f478(&local_78);
    }
    *pbVar4 = 0;
    pbVar4 = pbVar4 + 1;
    puVar10 = puVar10 + 8;
    puVar9 = puVar9 + 8;
    iVar3 = iVar3 + 0x10;
    puVar8 = puVar8 + 8;
    uVar5 = uVar5 + 1;
    puVar1 = puVar1 + 8;
  } while ((int)uVar5 < 0x18);
  SpuSetKey(0,(uint)CONCAT12((undefined1)DAT_800eaea2,DAT_800eaea0));
  SpuSetKey(1,(uint)CONCAT12((undefined1)DAT_800c440e,DAT_800c440c));
  SpuSetReverbVoice(8,(uint)CONCAT12((undefined1)DAT_800c4414,DAT_800c4412));
  DAT_800eaea0 = 0;
  DAT_800eaea2 = 0;
  DAT_800c440c = 0;
  DAT_800c440e = 0;
  return;
}


