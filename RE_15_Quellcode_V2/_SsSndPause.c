void _SsSndPause(short param_1,short param_2)

{
  int iVar1;
  int iVar2;
  
  iVar1 = param_2 * 0xac;
  iVar2 = (&DAT_800bb500)[param_1];
  SpuVmSeqKeyOff((int)param_2 << 8 | (int)param_1);
  *(undefined1 *)(iVar1 + iVar2 + 0x2b) = 0;
  iVar1 = iVar1 + (&DAT_800bb500)[param_1];
  *(uint *)(iVar1 + 0x90) = *(uint *)(iVar1 + 0x90) & 0xfffffffd;
  return;
}
