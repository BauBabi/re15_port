void _SsSndSetPauseMode(ushort param_1,short param_2)

{
  int iVar1;
  int *piVar2;
  
  piVar2 = (int *)((int)&DAT_800bb500 + ((int)((uint)param_1 << 0x10) >> 0xe));
  iVar1 = param_2 * 0xac;
  SpuVmGetSeqVol((int)(short)(param_1 | param_2 << 8),iVar1 + *piVar2 + 0x78,iVar1 + *piVar2 + 0x7a)
  ;
  *(uint *)(iVar1 + *piVar2 + 0x90) = *(uint *)(iVar1 + *piVar2 + 0x90) & 0xfffffffe;
  *(uint *)(iVar1 + *piVar2 + 0x90) = *(uint *)(iVar1 + *piVar2 + 0x90) & 0xfffffff7;
  *(uint *)(iVar1 + *piVar2 + 0x90) = *(uint *)(iVar1 + *piVar2 + 0x90) | 2;
  return;
}
