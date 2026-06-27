/* FUN_8002fe38 @ 0x8002fe38  (Ghidra headless, raw MIPS overlay) */

undefined4 FUN_8002fe38(undefined4 param_1,uint param_2,int param_3,undefined4 param_4)

{
  uint uVar1;
  
  if ((DAT_800e873c & 0x80) != 0) {
    return 0xffffffff;
  }
  DAT_800cfbd8 = DAT_800cfbd8 | 0x40;
  uVar1 = param_2 & 0x300;
  if (uVar1 == 0x200) {
    DAT_800e8758 = 0x403;
    if ((DAT_800cfb74 & 0x840) == 0x40) {
      DAT_800e8758 = 0x4403;
      DAT_800e8744 = &UNK_800a075c + *(ushort *)(&DAT_800a1e7c + param_3 * 2);
    }
    else {
      DAT_800e8744 = &UNK_8009f398 + *(ushort *)(&DAT_800a0674 + param_3 * 2);
    }
    DAT_800e873c = 0x80;
    DAT_800e873d = 0;
    DAT_800e873e = 0x80;
    DAT_800e8754 = 0x22;
    DAT_800e8756 = 0xba;
    DAT_800e875c = DAT_800cfbdc;
    DAT_800e8760 = param_4;
    return 0;
  }
  if (uVar1 < 0x201) {
    if (uVar1 == 0x100) {
      if ((DAT_800cfb74 & 0x840) == 0x40) {
        DAT_800e873c = 0x80;
        DAT_800e873d = 0;
        DAT_800e873e = 0x80;
        DAT_800e8744 = &UNK_8009efcc + *(ushort *)(&DAT_8009f368 + param_3 * 2);
        DAT_800e8754 = 0x22;
        DAT_800e8756 = 0xb9;
        DAT_800e8758 = 0x6003;
        DAT_800e875c = DAT_800cfbdc;
        DAT_800e8760 = param_4;
        return 0;
      }
      DAT_800e873c = 0x80;
      DAT_800e873d = 0;
      DAT_800e873e = 0x80;
      DAT_800e8744 = &UNK_8009ecc4 + *(ushort *)(&DAT_8009ef9c + param_3 * 2);
      DAT_800e8754 = 0x22;
      DAT_800e8756 = 0xb9;
      DAT_800e8758 = 0x2003;
      DAT_800e875c = DAT_800cfbdc;
      DAT_800e8760 = param_4;
      return 0;
    }
  }
  else if (uVar1 == 0x300) {
    DAT_800e8754 = 0x22;
    DAT_800e8756 = 0xb9;
    DAT_800e8758 = 0x803;
    DAT_800e873e = 0x80;
    if ((param_2 & 1) != 0) {
      DAT_800e8758 = 0x4803;
    }
    if ((DAT_800cfb74 & 0x840) == 0x40) {
      DAT_800e8758 = DAT_800e8758 | 0x4000;
      DAT_800e8744 = *(undefined **)(DAT_800ce324 + 0x40);
    }
    else {
      DAT_800e8744 = *(undefined **)(DAT_800ce324 + 0x3c);
    }
    goto LAB_800301fc;
  }
  DAT_800e873e = (byte)(param_2 >> 8) & 0x80;
  DAT_800e8756 = (undefined2)((uint)param_1 >> 0x10);
  uVar1 = param_2 & 0xc00;
  DAT_800e8754 = (undefined2)param_1;
  DAT_800e8758 = (ushort)param_2;
  if (uVar1 == 0x400) {
    if ((DAT_800cfb74 & 0x840) == 0x40) {
      DAT_800e873c = 0x80;
      DAT_800e873d = 0;
      DAT_800e8744 = &UNK_800a075c + *(ushort *)(&DAT_800a1e7c + param_3 * 2);
      DAT_800e8758 = DAT_800e8758 | 0x4000;
      DAT_800e875c = DAT_800cfbdc;
      DAT_800e8760 = param_4;
      return 0;
    }
    DAT_800e873c = 0x80;
    DAT_800e873d = 0;
    DAT_800e8744 = &UNK_8009f398 + *(ushort *)(&DAT_800a0674 + param_3 * 2);
    DAT_800e875c = DAT_800cfbdc;
    DAT_800e8760 = param_4;
    return 0;
  }
  if (uVar1 < 0x401) {
    if (uVar1 != 0) {
      DAT_800e873c = 0x80;
      DAT_800e873d = 0;
      DAT_800e875c = DAT_800cfbdc;
      DAT_800e8760 = param_4;
      return 0;
    }
    if ((DAT_800cfb74 & 0x840) == 0x40) {
      DAT_800e873c = 0x80;
      DAT_800e873d = 0;
      DAT_800e8744 = &UNK_8009efcc + *(ushort *)(&DAT_8009f368 + param_3 * 2);
      DAT_800e8758 = DAT_800e8758 | 0x4000;
      DAT_800e875c = DAT_800cfbdc;
      DAT_800e8760 = param_4;
      return 0;
    }
    DAT_800e873c = 0x80;
    DAT_800e873d = 0;
    DAT_800e8744 = &UNK_8009ecc4 + *(ushort *)(&DAT_8009ef9c + param_3 * 2);
    DAT_800e875c = DAT_800cfbdc;
    DAT_800e8760 = param_4;
    return 0;
  }
  if (uVar1 != 0x800) {
    if (uVar1 == 0xc00) {
      DAT_800e873c = 0x80;
      DAT_800e873d = 0;
      DAT_800e8744 = (undefined *)param_3;
      DAT_800e875c = DAT_800cfbdc;
      DAT_800e8760 = param_4;
      return 0;
    }
    DAT_800e873c = 0x80;
    DAT_800e873d = 0;
    DAT_800e875c = DAT_800cfbdc;
    DAT_800e8760 = param_4;
    return 0;
  }
  if ((DAT_800cfb74 & 0x840) == 0x40) {
    DAT_800e8744 = *(undefined **)(DAT_800ce324 + 0x40);
    DAT_800e8758 = DAT_800e8758 | 0x4000;
  }
  else {
    DAT_800e8744 = *(undefined **)(DAT_800ce324 + 0x3c);
  }
LAB_800301fc:
  DAT_800e8760 = param_4;
  DAT_800e875c = DAT_800cfbdc;
  DAT_800e8744 = (undefined *)
                 ((int)DAT_800e8744 + (uint)*(ushort *)((int)DAT_800e8744 + param_3 * 2));
  DAT_800e873d = 0;
  DAT_800e873c = 0x80;
  return 0;
}


