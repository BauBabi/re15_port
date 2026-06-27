/* S_SRMP_OBJ_180 @ 0x80080884  (Ghidra headless, raw MIPS overlay) */

undefined4 S_SRMP_OBJ_180(void)

{
  undefined1 uVar1;
  undefined4 uVar2;
  undefined *puVar3;
  undefined *puVar4;
  int iVar5;
  undefined4 *puVar6;
  int unaff_s2;
  uint unaff_s3;
  int unaff_s4;
  int unaff_s5;
  int unaff_s6;
  uint unaff_s7;
  int unaff_s8;
  uint in_stack_00000010;
  short sStack00000014;
  short sStack00000016;
  undefined4 in_stack_00000020;
  short sStack00000028;
  short sStack0000002a;
  short sStack0000002c;
  short sStack0000002e;
  short sStack00000034;
  short sStack00000036;
  short sStack00000048;
  short sStack0000004a;
  short sStack0000004c;
  short sStack0000004e;
  int in_stack_00000058;
  
  puVar4 = PTR_VOICE_00_LEFT_RIGHT_800ab21c;
  if ((((unaff_s5 != 0) || ((unaff_s3 & 8) != 0)) && (DAT_800ab29c < 9)) && (6 < DAT_800ab29c)) {
    unaff_s6 = 1;
    if (unaff_s4 == 0) {
      puVar6 = &stack0x00000010;
      iVar5 = 0x43;
      puVar3 = &DAT_800ab748 + DAT_800ab29c * 0x44;
      do {
        uVar1 = *puVar3;
        puVar3 = puVar3 + 1;
        iVar5 = iVar5 + -1;
        *(undefined1 *)puVar6 = uVar1;
        puVar6 = (undefined4 *)((int)puVar6 + 1);
      } while (iVar5 != -1);
      in_stack_00000010 = 0xc011c00;
    }
    DAT_800ab2a4 = *(int *)(unaff_s2 + 0xc);
    sStack00000028 = (short)((DAT_800ab2a4 * 0x2000) / 0x7f) - sStack00000014;
    sStack00000048 = (short)((DAT_800ab2a4 * 0x1000) / 0x7f);
    sStack0000002a = sStack00000048 - sStack00000016;
    sStack00000034 = sStack00000036 + sStack00000048;
    sStack0000002c = sStack0000002e + sStack00000048;
    sStack0000004a = sStack0000004e + sStack00000048;
    sStack00000048 = sStack0000004c + sStack00000048;
  }
  if (((unaff_s5 != 0) || ((unaff_s3 & 0x10) != 0)) && ((DAT_800ab29c < 9 && (6 < DAT_800ab29c)))) {
    unaff_s8 = 1;
    if (unaff_s4 == 0) {
      puVar6 = &stack0x00000010;
      if (unaff_s6 == 0) {
        iVar5 = 0x43;
        puVar4 = &DAT_800ab748 + DAT_800ab29c * 0x44;
        do {
          uVar1 = *puVar4;
          puVar4 = puVar4 + 1;
          iVar5 = iVar5 + -1;
          *(undefined1 *)puVar6 = uVar1;
          puVar6 = (undefined4 *)((int)puVar6 + 1);
        } while (iVar5 != -1);
        uVar2 = S_SRMP_OBJ_334();
        return uVar2;
      }
      in_stack_00000010 = in_stack_00000010 | 0x80;
    }
    DAT_800ab2a8 = *(int *)(unaff_s2 + 0x10);
    in_stack_00000020._2_2_ = (undefined2)((DAT_800ab2a8 * 0x8100) / 0x7f);
  }
  if (unaff_s4 == 0) {
    if ((unaff_s5 != 0) || ((unaff_s3 & 2) != 0)) {
      *(undefined2 *)(PTR_VOICE_00_LEFT_RIGHT_800ab21c + 0x184) = *(undefined2 *)(unaff_s2 + 8);
      DAT_800ab2a0 = *(undefined2 *)(unaff_s2 + 8);
    }
    if ((unaff_s5 != 0) || ((unaff_s3 & 4) != 0)) {
      *(undefined2 *)(PTR_VOICE_00_LEFT_RIGHT_800ab21c + 0x186) = *(undefined2 *)(unaff_s2 + 10);
      DAT_800ab2a2 = *(undefined2 *)(unaff_s2 + 10);
      uVar2 = S_SRMP_OBJ_444();
      return uVar2;
    }
  }
  else {
    unaff_s7 = *(ushort *)(PTR_VOICE_00_LEFT_RIGHT_800ab21c + 0x1aa) >> 7 & 1;
    if ((*(ushort *)(PTR_VOICE_00_LEFT_RIGHT_800ab21c + 0x1aa) >> 7 & 1) != 0) {
      *(ushort *)(PTR_VOICE_00_LEFT_RIGHT_800ab21c + 0x1aa) =
           *(ushort *)(PTR_VOICE_00_LEFT_RIGHT_800ab21c + 0x1aa) & 0xff7f;
      uVar2 = S_SRMP_OBJ_420();
      return uVar2;
    }
    *(undefined2 *)(PTR_VOICE_00_LEFT_RIGHT_800ab21c + 0x184) = 0;
    *(undefined2 *)(puVar4 + 0x186) = 0;
    DAT_800ab2a0 = 0;
    DAT_800ab2a2 = 0;
  }
  if (((unaff_s4 != 0) || (unaff_s6 != 0)) || (unaff_s8 != 0)) {
    _spu_setReverbAttr(&stack0x00000010);
  }
  if (in_stack_00000058 != 0) {
    SpuClearReverbWorkArea(DAT_800ab29c);
  }
  if ((unaff_s4 != 0) && (_spu_FsetRXX(0xd1,DAT_800ab294,0), unaff_s7 != 0)) {
    *(ushort *)(PTR_VOICE_00_LEFT_RIGHT_800ab21c + 0x1aa) =
         *(ushort *)(PTR_VOICE_00_LEFT_RIGHT_800ab21c + 0x1aa) | 0x80;
  }
  return 0;
}


