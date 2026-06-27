/* _SpuInit @ 0x800791f4  (Ghidra headless, raw MIPS overlay) */

/* WARNING: Unknown calling convention -- yet parameter storage is locked */

void _SpuInit(void)

{
  undefined2 *puVar1;
  int iVar2;
  int in_a0;
  
  ResetCallback();
  _spu_init();
  if (in_a0 == 0) {
    iVar2 = 0x17;
    puVar1 = &DAT_800ab2e2;
    do {
      *puVar1 = 0xc000;
      iVar2 = iVar2 + -1;
      puVar1 = puVar1 + -1;
    } while (-1 < iVar2);
  }
  SpuStart();
  DAT_800ab28c = 0;
  DAT_800ab290 = 0;
  DAT_800ab29c = 0;
  DAT_800ab2a0 = 0;
  DAT_800ab2a2 = 0;
  DAT_800ab2a4 = 0;
  DAT_800ab2a8 = 0;
  DAT_800ab294 = DAT_800ab6f8;
  _spu_FsetRXX(0xd1,DAT_800ab6f8,0);
  DAT_800ab6ec = 0;
  DAT_800ab6f0 = 0;
  DAT_800ab6f4 = 0;
  DAT_800ab288 = 0;
  DAT_800ab238 = 0;
  DAT_800ab284 = 0;
  DAT_800ab2b0 = 0;
  DAT_800ab2ac = 0;
  DAT_800ab6e4 = 0;
  return;
}


