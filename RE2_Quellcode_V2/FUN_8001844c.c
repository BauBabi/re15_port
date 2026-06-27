/* FUN_8001844c @ 0x8001844c  (Ghidra headless, raw MIPS overlay) */

void FUN_8001844c(uint *param_1)

{
  short sVar1;
  int iVar2;
  uint uVar3;
  undefined1 auStack_88 [96];
  
  uVar3 = param_1[0x66];
  if ((*param_1 & 0x40000) == 0) {
    RotMatrix((SVECTOR *)(param_1 + 0x1d),(MATRIX *)(param_1 + 9));
  }
  FUN_8002ce94(param_1 + 9,uVar3 + 0x18,auStack_88);
  FUN_8002ce94(auStack_88,uVar3 + 0x578,uVar3 + 0x5a8);
  sVar1 = *(short *)(uVar3 + 0x5ca) + *(short *)((int)param_1 + 0x76) + *(short *)(uVar3 + 0x6a);
  *(ushort *)(uVar3 + 0x6c) = *(ushort *)(uVar3 + 0x6c) & 0xfff;
  if ((param_1[0x70] & 1) == 0) {
    sVar1 = *(short *)(uVar3 + 0x5f8) + sVar1;
  }
  iVar2 = (int)sVar1;
  FUN_80018728(uVar3 + 0xa14,uVar3 + 0x5a8,400,100,0x28,2,0,400,100,0x18,4,0,iVar2);
  FUN_80018728(uVar3 + 0xac0,uVar3 + 0xa5c,0x15e,0,0x30,4,0x96,0x96,0,0x18,6,0,iVar2);
  FUN_80018728(uVar3 + 0xb6c,uVar3 + 0xb08,300,0,0x30,4,0,0x96,0,0x18,6,0,iVar2);
  FUN_80018728(uVar3 + 0xc18,uVar3 + 0xbb4,0xfa,0,0x30,4,0,100,0,0x18,6,0,iVar2);
  return;
}


