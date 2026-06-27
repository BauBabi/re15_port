void _SsContNrpn1(ushort param_1,short param_2,undefined1 param_3)

{
  undefined4 uVar1;
  char cVar2;
  int iVar3;
  
  iVar3 = param_2 * 0xac + *(int *)((int)&DAT_800bb500 + ((int)((uint)param_1 << 0x10) >> 0xe));
  if ((*(char *)(iVar3 + 0x27) == '\x01') && (*(char *)(iVar3 + 0x10) == '\0')) {
    *(undefined1 *)(iVar3 + 0x28) = param_3;
    *(undefined1 *)(iVar3 + 0x10) = 1;
    SEQREAD_OBJ_D84();
    return;
  }
  cVar2 = *(char *)(iVar3 + 0x16);
  if ((cVar2 != '\x1e') && (cVar2 != '\x14')) {
    *(undefined1 *)(iVar3 + 0x15) = param_3;
    *(char *)(iVar3 + 0x2a) = *(char *)(iVar3 + 0x2a) + '\x01';
    cVar2 = *(char *)(iVar3 + 0x16);
  }
  if (cVar2 == '(') {
    if (*(code **)(&UNK_800bb580 + param_2 * 4 + (short)param_1 * 0x40) != (code *)0x0) {
      (**(code **)(&UNK_800bb580 + param_2 * 4 + (short)param_1 * 0x40))
                ((int)(short)param_1,(int)param_2,param_3);
    }
  }
  uVar1 = _SsReadDeltaValue((int)(short)param_1,(int)param_2);
  *(undefined4 *)(iVar3 + 0x88) = uVar1;
  return;
}
