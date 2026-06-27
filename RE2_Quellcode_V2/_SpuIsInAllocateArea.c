/* _SpuIsInAllocateArea @ 0x8007938c  (Ghidra headless, raw MIPS overlay) */

undefined4 _SpuIsInAllocateArea(uint param_1)

{
  undefined4 uVar1;
  uint uVar2;
  uint *puVar3;
  
  puVar3 = DAT_800ab6f4;
  if (DAT_800ab6f4 == (uint *)0x0) {
    uVar1 = S_M_UTIL_OBJ_78();
    return uVar1;
  }
  do {
    uVar2 = *puVar3;
    if ((uVar2 & 0x80000000) == 0) {
      if ((uVar2 & 0x40000000) != 0) {
        return 0;
      }
      if (param_1 <= (uVar2 & 0xfffffff)) {
        return 1;
      }
      if (param_1 < (uVar2 & 0xfffffff) + puVar3[1]) {
        return 1;
      }
    }
    puVar3 = puVar3 + 2;
  } while( true );
}


