/* FUN_80015714 @ 0x80015714  (Ghidra headless, raw MIPS overlay) */

int FUN_80015714(int param_1,int param_2,int param_3)

{
  int iVar1;
  uint uVar2;
  int iVar3;
  
  uVar2 = param_3 + (param_2 - (uint)*(ushort *)(param_1 + 0x76)) & 0xfff;
  iVar3 = (int)(short)param_3;
  iVar1 = 0;
  if ((iVar3 << 1 <= (int)uVar2) && (iVar1 = iVar3, 0x800 < uVar2)) {
    iVar1 = -iVar3;
  }
  return iVar1;
}


