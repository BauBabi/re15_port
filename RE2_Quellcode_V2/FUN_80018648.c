/* FUN_80018648 @ 0x80018648  (Ghidra headless, raw MIPS overlay) */

void FUN_80018648(uint *param_1)

{
  ushort uVar1;
  uint uVar2;
  
  uVar2 = param_1[0x66];
  if ((*param_1 & 0x40000) == 0) {
    RotMatrix((SVECTOR *)(param_1 + 0x1d),(MATRIX *)(param_1 + 9));
  }
  FUN_8002ce94(param_1 + 9,uVar2 + 0x18,uVar2 + 0x48);
  uVar1 = *(ushort *)((int)param_1 + 0x76);
  *(ushort *)(uVar2 + 0x6c) = *(ushort *)(uVar2 + 0x6c) & 0xfff;
  FUN_80018728(uVar2 + 0xa14,uVar2 + 0x48,400,100,0x30,4,0,0x32,0,0x30,6,0,
               (int)(((uint)uVar1 + (uint)*(ushort *)(uVar2 + 0x6a)) * 0x10000) >> 0x10);
  return;
}


