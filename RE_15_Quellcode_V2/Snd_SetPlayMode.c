void Snd_SetPlayMode(ushort param_1,short param_2,char param_3,undefined2 param_4)

{
  int iVar1;
  int iVar2;
  int *piVar3;
  
  piVar3 = (int *)((int)&DAT_800bb500 + ((int)((uint)param_1 << 0x10) >> 0xe));
  iVar2 = param_2 * 0xac;
  iVar1 = iVar2 + *piVar3;
  *(uint *)(iVar1 + 0x90) = *(uint *)(iVar1 + 0x90) & 0xfffffdff;
  *(uint *)(iVar2 + *piVar3 + 0x90) = *(uint *)(iVar2 + *piVar3 + 0x90) & 0xfffffffb;
  *(undefined2 *)(iVar1 + 0x46) = param_4;
  if (param_3 == '\x01') {
    *(uint *)(iVar2 + *piVar3 + 0x90) = *(uint *)(iVar2 + *piVar3 + 0x90) | 1;
    *(undefined2 *)(iVar1 + 0x48) = 0;
    *(undefined1 *)(iVar1 + 0x2b) = 1;
    SpuVmSetSeqVol((int)(short)(param_1 | param_2 << 8),*(undefined2 *)(iVar1 + 0x74),
                   *(undefined2 *)(iVar1 + 0x76),0);
    SSPLAY_OBJ_F0();
    return;
  }
  if (param_3 == '\0') {
    *(uint *)(iVar2 + *piVar3 + 0x90) = *(uint *)(iVar2 + *piVar3 + 0x90) | 2;
  }
  return;
}
