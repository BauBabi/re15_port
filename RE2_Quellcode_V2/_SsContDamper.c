/* _SsContDamper @ 0x8007af3c  (Ghidra headless, raw MIPS overlay) */

void _SsContDamper(short param_1,short param_2,uchar param_3)

{
  undefined4 uVar1;
  int iVar2;
  
  iVar2 = *(int *)((int)&DAT_800ea250 + ((int)((uint)(ushort)param_1 << 0x10) >> 0xe));
  if (param_3 < 0x40) {
    FUN_80081efc();
    CC_64_OBJ_78((uint)(ushort)param_1 << 0x10);
    return;
  }
  FUN_80081f0c();
  uVar1 = _SsReadDeltaValue((int)param_1,(int)param_2);
  *(undefined4 *)(iVar2 + param_2 * 0xb0 + 0x90) = uVar1;
  return;
}


