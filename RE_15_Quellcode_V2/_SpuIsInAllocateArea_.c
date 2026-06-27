undefined4 _SpuIsInAllocateArea_(int param_1)

{
  uint uVar1;
  uint uVar2;
  uint *puVar3;
  
  uVar2 = param_1 << (DAT_80076d80 & 0x1f);
  puVar3 = DAT_80076dbc;
  do {
    uVar1 = *puVar3;
    if ((uVar1 & 0x80000000) == 0) {
      if ((uVar1 & 0x40000000) != 0) {
        return 0;
      }
      if (uVar2 <= (uVar1 & 0xfffffff)) {
        return 1;
      }
      if (uVar2 < (uVar1 & 0xfffffff) + puVar3[1]) {
        return 1;
      }
    }
    puVar3 = puVar3 + 2;
  } while( true );
}
