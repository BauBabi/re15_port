void SYS_OBJ_1E4(uint param_1)

{
  uint in_v0;
  undefined4 uVar1;
  
  _ctl(param_1 | in_v0);
  if (DAT_8007e350 == '\x02') {
    uVar1 = 0x20000504;
    if (DAT_8007e353 != '\0') {
      uVar1 = 0x20000501;
    }
    _ctl(uVar1);
  }
  return;
}
