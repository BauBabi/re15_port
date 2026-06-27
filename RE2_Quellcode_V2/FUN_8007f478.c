/* FUN_8007f478 @ 0x8007f478  (Ghidra headless, raw MIPS overlay) */

void FUN_8007f478(uint *param_1)

{
  bool bVar1;
  undefined2 uVar2;
  uint uVar3;
  ushort uVar4;
  ushort uVar5;
  uint uVar6;
  uint uVar7;
  int local_20;
  
  uVar6 = param_1[1];
  uVar7 = 0;
  bVar1 = uVar6 == 0;
  do {
    if ((*param_1 & 1 << (uVar7 & 0x1f)) != 0) {
      if ((bVar1) || ((uVar6 & 0x10) != 0)) {
        *(short *)(PTR_VOICE_00_LEFT_RIGHT_800ab21c + uVar7 * 0x10 + 4) = (short)param_1[5];
      }
      if ((bVar1) || ((uVar6 & 0x40) != 0)) {
        (&DAT_800ab2b4)[uVar7] = (short)param_1[6];
      }
      if ((bVar1) || ((uVar6 & 0x20) != 0)) {
        uVar2 = _spu_note2pitch((ushort)(&DAT_800ab2b4)[uVar7] >> 8,(&DAT_800ab2b4)[uVar7] & 0xff,
                                *(ushort *)((int)param_1 + 0x16) >> 8,
                                *(ushort *)((int)param_1 + 0x16) & 0xff);
        *(undefined2 *)(PTR_VOICE_00_LEFT_RIGHT_800ab21c + uVar7 * 0x10 + 4) = uVar2;
      }
      if ((bVar1) || ((uVar6 & 1) != 0)) {
        uVar5 = 0;
        uVar4 = (ushort)param_1[2] & 0x7fff;
        if ((bVar1) || ((uVar6 & 4) != 0)) {
          switch((int)(((ushort)param_1[3] - 1) * 0x10000) >> 0x10) {
          case 0:
            uVar5 = 0x8000;
            break;
          case 1:
            uVar5 = 0x9000;
            break;
          case 2:
            uVar5 = 0xa000;
            break;
          case 3:
            uVar5 = 0xb000;
            break;
          case 4:
            uVar5 = 0xc000;
            break;
          case 5:
            uVar5 = 0xd000;
            break;
          case 6:
            uVar5 = 0xe000;
          }
        }
        if (uVar5 != 0) {
          if ((short)param_1[2] < 0x80) {
            if ((short)param_1[2] < 0) {
              uVar4 = 0;
            }
          }
          else {
            uVar4 = 0x7f;
          }
        }
        *(ushort *)(PTR_VOICE_00_LEFT_RIGHT_800ab21c + uVar7 * 0x10) = uVar4 | uVar5;
      }
      if ((bVar1) || ((uVar6 & 2) != 0)) {
        uVar5 = 0;
        uVar4 = *(ushort *)((int)param_1 + 10) & 0x7fff;
        if ((bVar1) || ((uVar6 & 8) != 0)) {
          switch((int)((*(ushort *)((int)param_1 + 0xe) - 1) * 0x10000) >> 0x10) {
          case 0:
            uVar5 = 0x8000;
            break;
          case 1:
            uVar5 = 0x9000;
            break;
          case 2:
            uVar5 = 0xa000;
            break;
          case 3:
            uVar5 = 0xb000;
            break;
          case 4:
            uVar5 = 0xc000;
            break;
          case 5:
            uVar5 = 0xd000;
            break;
          case 6:
            uVar5 = 0xe000;
          }
        }
        if (uVar5 != 0) {
          if (*(short *)((int)param_1 + 10) < 0x80) {
            if (*(short *)((int)param_1 + 10) < 0) {
              uVar4 = 0;
            }
          }
          else {
            uVar4 = 0x7f;
          }
        }
        *(ushort *)(PTR_VOICE_00_LEFT_RIGHT_800ab21c + uVar7 * 0x10 + 2) = uVar4 | uVar5;
      }
      if ((bVar1) || ((uVar6 & 0x80) != 0)) {
        _spu_FsetRXXa(uVar7 << 3 | 3,param_1[7]);
      }
      if ((bVar1) || ((uVar6 & 0x10000) != 0)) {
        _spu_FsetRXXa(uVar7 << 3 | 7,param_1[8]);
      }
      if ((bVar1) || ((uVar6 & 0x20000) != 0)) {
        *(undefined2 *)(PTR_VOICE_00_LEFT_RIGHT_800ab21c + uVar7 * 0x10 + 8) =
             *(undefined2 *)((int)param_1 + 0x3a);
      }
      if ((bVar1) || ((uVar6 & 0x40000) != 0)) {
        *(short *)(PTR_VOICE_00_LEFT_RIGHT_800ab21c + uVar7 * 0x10 + 10) = (short)param_1[0xf];
      }
      if ((bVar1) || ((uVar6 & 0x800) != 0)) {
        uVar5 = (ushort)param_1[0xc];
        if (0x7f < uVar5) {
          uVar5 = 0x7f;
        }
        uVar4 = 0;
        if (((bVar1) || ((uVar6 & 0x100) != 0)) && (param_1[9] == 5)) {
          uVar4 = 0x80;
        }
        *(ushort *)(PTR_VOICE_00_LEFT_RIGHT_800ab21c + uVar7 * 0x10 + 8) =
             *(ushort *)(PTR_VOICE_00_LEFT_RIGHT_800ab21c + uVar7 * 0x10 + 8) & 0xff |
             (uVar5 | uVar4) << 8;
      }
      if ((bVar1) || ((uVar6 & 0x1000) != 0)) {
        uVar5 = *(ushort *)((int)param_1 + 0x32);
        if (0xf < uVar5) {
          uVar5 = 0xf;
        }
        *(ushort *)(PTR_VOICE_00_LEFT_RIGHT_800ab21c + uVar7 * 0x10 + 8) =
             *(ushort *)(PTR_VOICE_00_LEFT_RIGHT_800ab21c + uVar7 * 0x10 + 8) & 0xff0f | uVar5 << 4;
      }
      if ((bVar1) || ((uVar6 & 0x2000) != 0)) {
        uVar5 = (ushort)param_1[0xd];
        if (0x7f < uVar5) {
          uVar5 = 0x7f;
        }
        uVar4 = 0x100;
        if ((bVar1) || ((uVar6 & 0x200) != 0)) {
          uVar3 = param_1[10];
          if (uVar3 == 5) {
            uVar4 = 0x200;
          }
          else if ((int)uVar3 < 6) {
            if (uVar3 == 1) {
              uVar4 = 0;
            }
          }
          else if (uVar3 == 7) {
            uVar4 = 0x300;
          }
        }
        *(ushort *)(PTR_VOICE_00_LEFT_RIGHT_800ab21c + uVar7 * 0x10 + 10) =
             *(ushort *)(PTR_VOICE_00_LEFT_RIGHT_800ab21c + uVar7 * 0x10 + 10) & 0x3f |
             (uVar5 | uVar4) << 6;
      }
      if ((bVar1) || ((uVar6 & 0x4000) != 0)) {
        uVar5 = *(ushort *)((int)param_1 + 0x36);
        if (0x1f < uVar5) {
          uVar5 = 0x1f;
        }
        uVar4 = 0;
        if ((((bVar1) || ((uVar6 & 0x400) != 0)) && (param_1[0xb] != 3)) && (param_1[0xb] == 7)) {
          uVar4 = 0x20;
        }
        *(ushort *)(PTR_VOICE_00_LEFT_RIGHT_800ab21c + uVar7 * 0x10 + 10) =
             *(ushort *)(PTR_VOICE_00_LEFT_RIGHT_800ab21c + uVar7 * 0x10 + 10) & 0xffc0 |
             uVar5 | uVar4;
      }
      if ((bVar1) || ((uVar6 & 0x8000) != 0)) {
        uVar5 = (ushort)param_1[0xe];
        if (0xf < uVar5) {
          uVar5 = 0xf;
        }
        *(ushort *)(PTR_VOICE_00_LEFT_RIGHT_800ab21c + uVar7 * 0x10 + 8) =
             uVar5 | *(ushort *)(PTR_VOICE_00_LEFT_RIGHT_800ab21c + uVar7 * 0x10 + 8) & 0xfff0;
      }
    }
    uVar7 = uVar7 + 1;
  } while ((int)uVar7 < 0x18);
  for (local_20 = 0; local_20 < 2; local_20 = local_20 + 1) {
  }
  return;
}


