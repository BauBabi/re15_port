/* FUN_80015614 @ 0x80015614  (Ghidra headless, raw MIPS overlay) */

int FUN_80015614(int param_1,short param_2,short param_3,int param_4)

{
  int iVar1;
  uint uVar2;
  int iVar3;
  
  iVar1 = FUN_800154ac((int)*(short *)(param_1 + 0x38),(int)*(short *)(param_1 + 0x40),(int)param_2,
                       (int)param_3);
  uVar2 = param_4 + (iVar1 - (uint)*(ushort *)(param_1 + 0x76)) & 0xfff;
  iVar3 = (int)(short)param_4;
  iVar1 = 0;
  if ((iVar3 << 1 <= (int)uVar2) && (iVar1 = iVar3, 0x800 < uVar2)) {
    iVar1 = -iVar3;
  }
  return iVar1;
}


