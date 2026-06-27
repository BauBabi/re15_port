/* FUN_80016200 @ 0x80016200  (Ghidra headless, raw MIPS overlay) */

void FUN_80016200(int param_1,int param_2,int param_3)

{
  long lVar1;
  int iVar2;
  undefined1 auStack_38 [20];
  int local_24;
  int local_1c;
  
  iVar2 = *(int *)(param_1 + 0x198);
  RotMatrix((SVECTOR *)(param_1 + 0x74),(MATRIX *)(param_1 + 0x24));
  FUN_8002ce94((MATRIX *)(param_1 + 0x24),iVar2 + 0x18,auStack_38);
  FUN_8002ce94(auStack_38,iVar2 + 0xc4,auStack_38);
  iVar2 = iVar2 + param_3 * 0x204 + 0x2b0;
  FUN_8002ce94(auStack_38,iVar2 + -0x140,auStack_38);
  FUN_8002ce94(auStack_38,iVar2 + -0x94,auStack_38);
  FUN_8002ce94(auStack_38,iVar2 + 0x18,auStack_38);
  if (param_2 == 0) {
    *(int *)(param_1 + 0x38) = *(int *)(param_1 + 0x38) - (local_24 - *(int *)(iVar2 + 0x5c));
    *(int *)(param_1 + 0x40) = *(int *)(param_1 + 0x40) - (local_1c - *(int *)(iVar2 + 100));
  }
  else {
    local_24 = local_24 - *(int *)(iVar2 + 0x5c);
    local_1c = local_1c - *(int *)(iVar2 + 100);
    lVar1 = SquareRoot0(local_24 * local_24 + local_1c * local_1c);
    *(short *)(param_1 + 0x144) = (short)lVar1;
  }
  return;
}


