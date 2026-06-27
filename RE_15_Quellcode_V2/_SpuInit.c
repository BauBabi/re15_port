void _SpuInit(void)

{
  undefined2 *puVar1;
  int iVar2;
  int in_a0;
  
  ResetCallback();
  _spu_init();
  if (in_a0 == 0) {
    iVar2 = 0x17;
    puVar1 = &DAT_80076e16;
    do {
      *puVar1 = 0xc000;
      iVar2 = iVar2 + -1;
      puVar1 = puVar1 + -1;
    } while (-1 < iVar2);
  }
  SpuStart();
  DAT_80076dc8 = 0;
  DAT_80076dcc = 0;
  DAT_80076dd8 = 0;
  DAT_80076ddc = 0;
  DAT_80076dde = 0;
  DAT_80076de0 = 0;
  DAT_80076de4 = 0;
  DAT_80076dd0 = 0xfffe;
  _spu_FsetRXX(0xd1,0xfffe,0);
  DAT_80076dc4 = 0;
  DAT_80076d74 = 0;
  DAT_80076e1c = 0;
  return;
}
