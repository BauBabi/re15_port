/* FUN_80095864 @ 0x80095864  (Ghidra headless, raw MIPS overlay) */

undefined4 FUN_80095864(uint param_1,undefined2 param_2,uint param_3)

{
  undefined *puVar1;
  undefined4 uVar2;
  ushort uVar3;
  
  puVar1 = PTR_TMR_DOTCLOCK_VAL_800c3978;
  param_1 = param_1 & 0xffff;
  uVar3 = 0x48;
  if (param_1 < 3) {
    *(undefined2 *)(PTR_TMR_DOTCLOCK_VAL_800c3978 + param_1 * 0x10 + 4) = 0;
    *(undefined2 *)(puVar1 + param_1 * 0x10 + 8) = param_2;
    if (param_1 < 2) {
      if ((param_3 & 0x10) != 0) {
        uVar3 = 0x49;
      }
      if ((param_3 & 1) == 0) {
        uVar3 = uVar3 | 0x100;
      }
    }
    else if ((param_1 == 2) && ((param_3 & 1) == 0)) {
      uVar3 = 0x248;
    }
    uVar2 = 1;
    if ((param_3 & 0x1000) != 0) {
      uVar3 = uVar3 | 0x10;
    }
    *(ushort *)(PTR_TMR_DOTCLOCK_VAL_800c3978 + param_1 * 0x10 + 4) = uVar3;
  }
  else {
    uVar2 = 0;
  }
  return uVar2;
}


