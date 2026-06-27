/* FUN_800428cc @ 0x800428cc  (Ghidra headless, raw MIPS overlay) */

void FUN_800428cc(int param_1)

{
  short sVar1;
  int iVar2;
  
  if ((*(ushort *)(param_1 + 0x10e) & 0xfff) - 9 < 3) {
    iVar2 = *(int *)(param_1 + 0x198) + 0xd70;
    if (*(char *)(param_1 + 8) == '\x0e') {
      iVar2 = *(int *)(param_1 + 0x198) + 0xb6c;
    }
    if (*(short *)(iVar2 + 0x86) != 0xff) {
      sVar1 = *(short *)(&DAT_800a6ea8 + *(char *)(iVar2 + 0x7a) * 2);
      *(short *)(iVar2 + 0x6c) = sVar1;
      if (sVar1 == -0x8000) {
        *(undefined2 *)(iVar2 + 0x6c) = 0;
        *(undefined2 *)(iVar2 + 0x86) = 0xff;
      }
      else {
        *(char *)(iVar2 + 0x7a) = *(char *)(iVar2 + 0x7a) + '\x01';
        RotMatrix((SVECTOR *)(iVar2 + 0x68),(MATRIX *)(iVar2 + 0x18));
      }
    }
  }
  return;
}


