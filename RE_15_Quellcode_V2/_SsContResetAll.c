void _SsContResetAll(short param_1,short param_2)

{
  undefined4 uVar1;
  int iVar2;
  
  iVar2 = param_2 * 0xac + (&DAT_800bb500)[param_1];
  FUN_8005f87c();
  FUN_8005fb00();
  *(undefined1 *)(iVar2 + (uint)*(byte *)(iVar2 + 0x12) + 0x2c) = *(undefined1 *)(iVar2 + 0x12);
  *(undefined1 *)(iVar2 + 0x13) = 0;
  *(undefined1 *)(iVar2 + 0x14) = 0;
  *(undefined2 *)((uint)*(byte *)(iVar2 + 0x12) * 2 + iVar2 + 0x4e) = 0x7f;
                    /* Possible PsyQ macro: setLineF2() */
  *(undefined1 *)(iVar2 + (uint)*(byte *)(iVar2 + 0x12) + 0x17) = 0x40;
  uVar1 = _SsReadDeltaValue((int)param_1,(int)param_2);
  *(undefined4 *)(iVar2 + 0x88) = uVar1;
  return;
}
