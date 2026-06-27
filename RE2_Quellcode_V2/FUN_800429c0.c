/* FUN_800429c0 @ 0x800429c0  (Ghidra headless, raw MIPS overlay) */

void FUN_800429c0(int param_1)

{
  ushort uVar1;
  short sVar2;
  int iVar3;
  
  if ((*(ushort *)(param_1 + 0x10e) & 0xfff) == 0x12) {
    iVar3 = *(int *)(param_1 + 0x198) + 0xac0;
    if (((*(byte *)(param_1 + 8) & 1) != 0) && (*(byte *)(param_1 + 8) < 10)) {
      iVar3 = *(int *)(param_1 + 0x198) + 0xd70;
    }
    if (*(char *)(param_1 + 8) == '\x0e') {
      iVar3 = iVar3 + 0xac;
    }
    if (*(char *)(iVar3 + 0x7a) == '\0') {
      uVar1 = *(short *)(iVar3 + 0x86) - 8;
      *(ushort *)(iVar3 + 0x86) = uVar1;
      if ((int)((uint)uVar1 << 0x10) < 0) {
        *(undefined2 *)(iVar3 + 0x86) = 0;
      }
    }
    else {
      sVar2 = *(short *)(iVar3 + 0x86) + 0x20;
      *(short *)(iVar3 + 0x86) = sVar2;
      if (0x200 < sVar2) {
        *(undefined2 *)(iVar3 + 0x86) = 0x200;
      }
    }
    *(short *)(iVar3 + 0x6a) = *(short *)(iVar3 + 0x6a) + *(short *)(iVar3 + 0x86);
    RotMatrix((SVECTOR *)(iVar3 + 0x68),(MATRIX *)(iVar3 + 0x18));
    *(undefined1 *)(iVar3 + 0x7a) = 0;
  }
  return;
}


