undefined4 S_SRMP_OBJ_170(void)

{
  undefined1 uVar1;
  undefined4 uVar2;
  undefined *puVar3;
  int iVar4;
  undefined4 *puVar5;
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
  undefined2 uStack00000022;
  short in_stack_00000028;
  short sStack0000002a;
  short sStack0000002c;
  short sStack0000002e;
  short sStack00000034;
  short sStack00000036;
  short in_stack_00000048;
  short sStack0000004a;
  short sStack0000004c;
  short sStack0000004e;
  int in_stack_00000058;
  
  DAT_80076de4 = 0;
  if ((((unaff_s5 != 0) || ((unaff_s3 & 8) != 0)) && (DAT_80076dd8 < 9)) && (6 < DAT_80076dd8)) {
    if (unaff_s4 == 0) {
      puVar5 = &stack0x00000010;
      iVar4 = 0x43;
      puVar3 = &DAT_80077274 + DAT_80076dd8 * 0x44;
      do {
        uVar1 = *puVar3;
        puVar3 = puVar3 + 1;
        iVar4 = iVar4 + -1;
        *(undefined1 *)puVar5 = uVar1;
        puVar5 = (undefined4 *)((int)puVar5 + 1);
      } while (iVar4 != -1);
      in_stack_00000010 = 0xc011c00;
    }
    DAT_80076de0 = *(int *)(unaff_s2 + 0xc);
    in_stack_00000028 = (short)((DAT_80076de0 * 0x2000) / 0x7f) - sStack00000014;
    sStack0000004a = (short)((DAT_80076de0 * 0x1000) / 0x7f);
    sStack0000002a = sStack0000004a - sStack00000016;
    sStack00000034 = sStack0000004a + sStack00000036;
    sStack0000002c = sStack0000004a + sStack0000002e;
    in_stack_00000048 = sStack0000004a + sStack0000004c;
    sStack0000004a = sStack0000004a + sStack0000004e;
    uVar2 = S_SRMP_OBJ_2AC();
    return uVar2;
  }
  DAT_80076de0 = 0;
  if (((unaff_s5 != 0) || ((unaff_s3 & 0x10) != 0)) && ((DAT_80076dd8 < 9 && (6 < DAT_80076dd8)))) {
    if (unaff_s4 == 0) {
      puVar5 = &stack0x00000010;
      if (unaff_s6 == 0) {
        iVar4 = 0x43;
        puVar3 = &DAT_80077274 + DAT_80076dd8 * 0x44;
        do {
          uVar1 = *puVar3;
          puVar3 = puVar3 + 1;
          iVar4 = iVar4 + -1;
          *(undefined1 *)puVar5 = uVar1;
          puVar5 = (undefined4 *)((int)puVar5 + 1);
        } while (iVar4 != -1);
        uVar2 = S_SRMP_OBJ_340();
        return uVar2;
      }
      in_stack_00000010 = in_stack_00000010 | 0x80;
    }
    DAT_80076de4 = *(int *)(unaff_s2 + 0x10);
    uStack00000022 = (undefined2)((DAT_80076de4 * 0x8100) / 0x7f);
    uVar2 = S_SRMP_OBJ_38C();
    return uVar2;
  }
  DAT_80076de4 = 0;
  if (unaff_s4 == 0) {
    if ((unaff_s5 != 0) || ((unaff_s3 & 2) != 0)) {
      SPU_REVERB_OUT_L = *(word *)(unaff_s2 + 8);
      DAT_80076ddc = *(undefined2 *)(unaff_s2 + 8);
    }
    if ((unaff_s5 != 0) || ((unaff_s3 & 4) != 0)) {
      SPU_REVERB_OUT_R = *(word *)(unaff_s2 + 10);
      DAT_80076dde = *(undefined2 *)(unaff_s2 + 10);
      uVar2 = S_SRMP_OBJ_45C();
      return uVar2;
    }
  }
  else {
    unaff_s7 = SPU_CTRL_REG_CPUCNT >> 7 & 1;
    if ((SPU_CTRL_REG_CPUCNT >> 7 & 1) != 0) {
      SPU_CTRL_REG_CPUCNT = SPU_CTRL_REG_CPUCNT & 0xff7f;
      uVar2 = S_SRMP_OBJ_438();
      return uVar2;
    }
    SPU_REVERB_OUT_L = 0;
    SPU_REVERB_OUT_R = 0;
    DAT_80076ddc = 0;
    DAT_80076dde = 0;
  }
  if (((unaff_s4 != 0) || (unaff_s6 != 0)) || (unaff_s8 != 0)) {
    _spu_setReverbAttr(&stack0x00000010);
  }
  if (in_stack_00000058 != 0) {
    SpuClearReverbWorkArea(DAT_80076dd8);
  }
  if ((unaff_s4 != 0) && (_spu_FsetRXX(0xd1,DAT_80076dd0,0), unaff_s7 != 0)) {
    SPU_CTRL_REG_CPUCNT = SPU_CTRL_REG_CPUCNT | 0x80;
  }
  return 0;
}
