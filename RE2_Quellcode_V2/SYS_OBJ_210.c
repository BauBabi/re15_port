/* SYS_OBJ_210 @ 0x800900f0  (Ghidra headless, raw MIPS overlay) */

void SYS_OBJ_210(uint param_1)

{
  uint in_v0;
  undefined4 uVar1;
  
  (**(code **)(PTR_PTR_800b26f8 + 0x10))(param_1 | in_v0);
  if (DAT_800b2700 == '\x02') {
    uVar1 = 0x20000504;
    if (DAT_800b2703 != '\0') {
      uVar1 = 0x20000501;
    }
    (**(code **)(PTR_PTR_800b26f8 + 0x10))(uVar1);
  }
  return;
}


