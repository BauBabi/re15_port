/* FUN_80036e30 @ 0x80036e30  (Ghidra headless, raw MIPS overlay) */

void FUN_80036e30(int param_1)

{
  undefined2 uVar1;
  undefined2 uVar2;
  undefined2 uVar3;
  
  if ((*(ushort *)(param_1 + 0x76) & 0x400) == 0) {
    uVar1 = *(undefined2 *)(param_1 + 0x96);
    uVar2 = *(undefined2 *)(param_1 + 0x9a);
    uVar3 = *(undefined2 *)(param_1 + 0x9c);
    *(undefined2 *)(param_1 + 0xa0) = *(undefined2 *)(param_1 + 0x94);
  }
  else {
    uVar1 = *(undefined2 *)(param_1 + 0x94);
    uVar2 = *(undefined2 *)(param_1 + 0x9c);
    uVar3 = *(undefined2 *)(param_1 + 0x9a);
    *(undefined2 *)(param_1 + 0xa0) = *(undefined2 *)(param_1 + 0x96);
  }
  *(undefined2 *)(param_1 + 0xa2) = uVar1;
  *(undefined2 *)(param_1 + 0x90) = uVar2;
  *(undefined2 *)(param_1 + 0x92) = uVar3;
  *(int *)(param_1 + 0x84) = *(int *)(param_1 + 0x38) + (int)*(short *)(param_1 + 0xa0);
  *(int *)(param_1 + 0x88) = *(int *)(param_1 + 0x3c) + (int)*(short *)(param_1 + 0x98);
  *(int *)(param_1 + 0x8c) = *(int *)(param_1 + 0x40) + (int)*(short *)(param_1 + 0xa2);
  return;
}


