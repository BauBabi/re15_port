void VOL_OBJ_348(void)

{
  int iVar1;
  int *piVar2;
  int in_t0;
  short in_t1;
  
  piVar2 = (int *)((int)&DAT_800bb500 + ((in_t0 << 0x10) >> 0xe));
  iVar1 = in_t1 * 0xac + *piVar2;
  *(uint *)(iVar1 + 0x90) = *(uint *)(iVar1 + 0x90) | 0x20;
  iVar1 = in_t1 * 0xac + *piVar2;
  *(uint *)(iVar1 + 0x90) = *(uint *)(iVar1 + 0x90) & 0xffffffef;
  return;
}
