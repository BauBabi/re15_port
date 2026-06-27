/* SetGraphReverse @ 0x80090064  (Ghidra headless, raw MIPS overlay) */

uint SetGraphReverse(undefined4 param_1)

{
  uint uVar1;
  undefined4 uVar2;
  uint uVar3;
  
  uVar3 = (uint)DAT_800b2703;
  if (1 < DAT_800b2702) {
    (*(code *)PTR_printf_800b26fc)("SetGraphReverse(%d)...\n",param_1);
  }
  DAT_800b2703 = (byte)param_1;
  uVar1 = (**(code **)(PTR_PTR_800b26f8 + 0x28))(8);
  if (DAT_800b2703 != '\0') {
    uVar3 = SYS_OBJ_210();
    return uVar3;
  }
  (**(code **)(PTR_PTR_800b26f8 + 0x10))(uVar1 | 0x8000000);
  if (DAT_800b2700 == '\x02') {
    uVar2 = 0x20000504;
    if (DAT_800b2703 != '\0') {
      uVar2 = 0x20000501;
    }
    (**(code **)(PTR_PTR_800b26f8 + 0x10))(uVar2);
  }
  return uVar3;
}


