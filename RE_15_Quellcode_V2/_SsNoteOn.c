void _SsNoteOn(ushort param_1,short param_2,undefined1 param_3,byte param_4)

{
  int iVar1;
  int iVar2;
  
  iVar2 = param_2 * 0xac + *(int *)((int)&DAT_800bb500 + ((int)((uint)param_1 << 0x10) >> 0xe));
  iVar1 = iVar2 + (uint)*(byte *)(iVar2 + 0x12);
  if ((((int)*(short *)(iVar2 + 0xaa) >> (*(byte *)(iVar2 + 0x12) & 0x1f) & 1U) == 0) &&
     (*(short *)(iVar2 + 0x74) != 0)) {
    if (param_4 != 0) {
      SpuVmKeyOn((int)(short)(param_1 | param_2 << 8),(int)*(short *)(iVar2 + 0x4c),
                 *(undefined1 *)(iVar1 + 0x2c),param_3,param_4,*(undefined1 *)(iVar1 + 0x17));
      *(ushort *)(iVar2 + 0xa8) = (ushort)param_4;
      SEQREAD_OBJ_494();
      return;
    }
    SpuVmKeyOff((int)(short)(param_1 | param_2 << 8),(int)*(short *)(iVar2 + 0x4c),
                *(undefined1 *)(iVar1 + 0x2c),param_3);
  }
  return;
}
