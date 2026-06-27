/* _SsContNrpn1 @ 0x8007b06c  (Ghidra headless, raw MIPS overlay) */

void _SsContNrpn1(short param_1,short param_2,uchar param_3)

{
  undefined4 uVar1;
  char cVar2;
  int iVar3;
  
  iVar3 = *(int *)((int)&DAT_800ea250 + ((int)((uint)(ushort)param_1 << 0x10) >> 0xe)) +
          param_2 * 0xb0;
  if ((*(char *)(iVar3 + 0x1c) == '\x01') && (*(char *)(iVar3 + 0x15) == '\0')) {
    *(uchar *)(iVar3 + 0x1d) = param_3;
    *(undefined1 *)(iVar3 + 0x1c) = 0;
    *(undefined1 *)(iVar3 + 0x15) = 1;
    CC_98_OBJ_AC();
    return;
  }
  cVar2 = *(char *)(iVar3 + 0x1b);
  if ((cVar2 != '\x1e') && (cVar2 != '\x14')) {
    *(uchar *)(iVar3 + 0x1a) = param_3;
    *(undefined1 *)(iVar3 + 0x1c) = 0;
    *(char *)(iVar3 + 0x1f) = *(char *)(iVar3 + 0x1f) + '\x01';
    cVar2 = *(char *)(iVar3 + 0x1b);
  }
  if (cVar2 == '(') {
    if (*(code **)(&UNK_800ea2d0 + param_2 * 4 + param_1 * 0x40) != (code *)0x0) {
      (**(code **)(&UNK_800ea2d0 + param_2 * 4 + param_1 * 0x40))((int)param_1,(int)param_2,param_3)
      ;
    }
  }
  uVar1 = _SsReadDeltaValue((int)param_1,(int)param_2);
  *(undefined4 *)(iVar3 + 0x90) = uVar1;
  return;
}


