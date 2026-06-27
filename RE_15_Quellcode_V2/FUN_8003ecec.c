void FUN_8003ecec(void)

{
  int iVar1;
  uint uVar2;
  
  uVar2 = 0;
  iVar1 = 0;
  do {
    (&DAT_800b2b4d)[iVar1] = 0;
    (&DAT_800b2b4e)[iVar1] = 0;
    (&DAT_800b2b50)[iVar1] = 0xff;
    (&DAT_800b2b54)[iVar1] = 0xff;
    uVar2 = uVar2 + 1;
    iVar1 = iVar1 + 0x170;
  } while (uVar2 < 10);
  FUN_8003edbc();
  DAT_800b0ff4 = 0xff;
  DAT_800b1028 = 0;
  DAT_800b0ff2 = 0;
  DAT_800aca50 = 0;
  DAT_800aca52 = 0;
  DAT_800b281e = 0;
  DAT_800aca3c = DAT_800aca3c & 0xfffff3ff;
  return;
}
