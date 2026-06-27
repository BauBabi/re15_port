/* _SpuIsInAllocateArea_ @ 0x8007940c  (Ghidra headless, raw MIPS overlay) */

undefined4 _SpuIsInAllocateArea_(int param_1)

{
  undefined4 uVar1;
  uint uVar2;
  uint uVar3;
  uint *puVar4;
  
  uVar3 = param_1 << (DAT_800ab244 & 0x1f);
  puVar4 = DAT_800ab6f4;
  if (DAT_800ab6f4 == (uint *)0x0) {
    uVar1 = S_M_UTIL_OBJ_104();
    return uVar1;
  }
  do {
    uVar2 = *puVar4;
    if ((uVar2 & 0x80000000) == 0) {
      if ((uVar2 & 0x40000000) != 0) {
        return 0;
      }
      if (uVar3 <= (uVar2 & 0xfffffff)) {
        return 1;
      }
      if (uVar3 < (uVar2 & 0xfffffff) + puVar4[1]) {
        return 1;
      }
    }
    puVar4 = puVar4 + 2;
  } while( true );
}


