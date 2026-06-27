/* FUN_8007975c @ 0x8007975c  (Ghidra headless, raw MIPS overlay) */

void FUN_8007975c(uint *param_1)

{
  undefined *puVar1;
  uint uVar2;
  int iVar3;
  ushort uVar4;
  uint uVar5;
  undefined2 uVar6;
  ushort uVar7;
  undefined2 uVar8;
  ushort uVar9;
  uint uVar10;
  
  uVar5 = 0;
  uVar2 = 1;
  do {
    uVar10 = uVar5;
    if ((*param_1 & uVar2) != 0) break;
    uVar5 = uVar5 + 1;
    uVar2 = 1 << (uVar5 & 0x1f);
    uVar10 = 0xffffffff;
  } while ((int)uVar5 < 0x18);
  uVar8 = 0;
  if (uVar10 == 0xffffffff) {
    return;
  }
  uVar7 = *(ushort *)(PTR_VOICE_00_LEFT_RIGHT_800ab21c + uVar10 * 0x10);
  uVar9 = *(ushort *)((int)(PTR_VOICE_00_LEFT_RIGHT_800ab21c + uVar10 * 0x10) + 2);
  if ((uVar7 & 0x8000) == 0) goto LAB_800798a4;
  uVar4 = uVar7 & 0xf000;
  if (uVar4 == 0xb000) {
    uVar8 = 4;
LAB_800798a0:
    uVar7 = uVar7 & 0xfff;
  }
  else {
    if (0xb000 < uVar4) {
      if (uVar4 != 0xd000) {
        if (uVar4 < 0xd001) {
          if (uVar4 == 0xc000) {
            uVar8 = 5;
            goto LAB_800798a0;
          }
          uVar7 = uVar7 & 0xfff;
        }
        else {
          if ((uVar4 == 0xe000) || (uVar4 == 0xf000)) {
            uVar8 = 7;
            goto LAB_800798a0;
          }
          uVar7 = uVar7 & 0xfff;
        }
        goto LAB_800798a4;
      }
      uVar8 = 6;
      goto LAB_800798a0;
    }
    if (uVar4 == 0x9000) {
      uVar8 = 2;
      goto LAB_800798a0;
    }
    if (uVar4 < 0x9001) {
      if (uVar4 == 0x8000) {
        uVar8 = 1;
        goto LAB_800798a0;
      }
      uVar7 = uVar7 & 0xfff;
    }
    else {
      if (uVar4 == 0xa000) {
        uVar8 = 3;
        goto LAB_800798a0;
      }
      uVar7 = uVar7 & 0xfff;
    }
  }
LAB_800798a4:
  uVar6 = 0;
  if ((uVar9 & 0x8000) == 0) goto LAB_80079974;
  uVar4 = uVar9 & 0xf000;
  if (uVar4 == 0xb000) {
    uVar6 = 4;
  }
  else if (uVar4 < 0xb001) {
    if (uVar4 == 0x9000) {
      uVar6 = 2;
    }
    else if (uVar4 < 0x9001) {
      if (uVar4 != 0x8000) {
        uVar9 = uVar9 & 0xfff;
        goto LAB_80079974;
      }
      uVar6 = 1;
    }
    else {
      if (uVar4 != 0xa000) {
        uVar9 = uVar9 & 0xfff;
        goto LAB_80079974;
      }
      uVar6 = 3;
    }
  }
  else if (uVar4 == 0xd000) {
    uVar6 = 6;
  }
  else if (uVar4 < 0xd001) {
    if (uVar4 != 0xc000) {
      uVar9 = uVar9 & 0xfff;
      goto LAB_80079974;
    }
    uVar6 = 5;
  }
  else {
    if ((uVar4 != 0xe000) && (uVar4 != 0xf000)) {
      uVar9 = uVar9 & 0xfff;
      goto LAB_80079974;
    }
    uVar6 = 7;
  }
  uVar9 = uVar9 & 0xfff;
LAB_80079974:
  if (uVar7 < 0x4000) {
    *(ushort *)(param_1 + 2) = uVar7;
  }
  else {
    *(ushort *)(param_1 + 2) = uVar7 + 0x8000;
  }
  if (uVar9 < 0x4000) {
    *(ushort *)((int)param_1 + 10) = uVar9;
  }
  else {
    *(ushort *)((int)param_1 + 10) = uVar9 + 0x8000;
  }
  *(undefined2 *)((int)param_1 + 0xe) = uVar6;
  puVar1 = PTR_VOICE_00_LEFT_RIGHT_800ab21c;
  *(undefined2 *)(param_1 + 3) = uVar8;
  *(undefined2 *)(param_1 + 4) = *(undefined2 *)(puVar1 + uVar10 * 4 + 0x200);
  *(undefined2 *)((int)param_1 + 0x12) = *(undefined2 *)(puVar1 + uVar10 * 4 + 0x202);
  *(undefined2 *)(param_1 + 5) = *(undefined2 *)(puVar1 + uVar10 * 0x10 + 4);
  iVar3 = _spu_pitch2note((ushort)(&DAT_800ab2b4)[uVar10] >> 8,(&DAT_800ab2b4)[uVar10] & 0xff);
  if (iVar3 < 0) {
    *(undefined2 *)((int)param_1 + 0x16) = 0;
  }
  else {
    *(short *)((int)param_1 + 0x16) = (short)iVar3;
  }
  *(undefined2 *)(param_1 + 6) = (&DAT_800ab2b4)[uVar10];
  uVar2 = DAT_800ab244;
  puVar1 = PTR_VOICE_00_LEFT_RIGHT_800ab21c;
  *(undefined2 *)((int)param_1 + 0x1a) =
       *(undefined2 *)(PTR_VOICE_00_LEFT_RIGHT_800ab21c + uVar10 * 0x10 + 0xc);
  param_1[7] = (uint)*(ushort *)(puVar1 + uVar10 * 0x10 + 6) << (uVar2 & 0x1f);
  param_1[8] = (uint)*(ushort *)(puVar1 + uVar10 * 0x10 + 0xe) << (uVar2 & 0x1f);
  uVar7 = *(ushort *)(puVar1 + uVar10 * 0x10 + 8);
  uVar9 = *(ushort *)(puVar1 + uVar10 * 0x10 + 10);
  uVar2 = 5;
  if ((uVar7 & 0x8000) == 0) {
    uVar2 = 1;
  }
  param_1[9] = uVar2;
  uVar4 = uVar9 & 0xe000;
  if (uVar4 == 0xc000) {
    uVar2 = 7;
  }
  else if (uVar4 == 0x8000) {
    uVar2 = 5;
  }
  else {
    uVar2 = 1;
    if (uVar4 == 0x4000) {
      uVar2 = 3;
    }
  }
  param_1[10] = uVar2;
  uVar2 = 7;
  if ((uVar9 & 0x20) == 0) {
    uVar2 = 3;
  }
  param_1[0xb] = uVar2;
  *(ushort *)(param_1 + 0xc) = uVar7 >> 8 & 0x3f;
  *(short *)((int)param_1 + 0x32) = (short)((uVar7 & 0xf0) >> 4);
  *(ushort *)(param_1 + 0xd) = uVar9 >> 6 & 0x7f;
  *(ushort *)((int)param_1 + 0x36) = uVar9 & 0x1f;
  *(ushort *)(param_1 + 0xe) = uVar7 & 0xf;
  *(ushort *)((int)param_1 + 0x3a) = uVar7;
  *(ushort *)(param_1 + 0xf) = uVar9;
  return;
}


