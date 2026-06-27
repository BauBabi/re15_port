/* FUN_80042aac @ 0x80042aac  (Ghidra headless, raw MIPS overlay) */

void FUN_80042aac(int param_1)

{
  int iVar1;
  int iVar2;
  undefined2 local_20;
  undefined2 local_1e;
  undefined2 local_1c;
  
  iVar2 = *(int *)(param_1 + 0x198);
  FUN_8002ce94(param_1 + 0x24,iVar2 + 0x18,iVar2 + 0x48);
  local_20 = (undefined2)*(undefined4 *)(iVar2 + 0x5c);
  local_1e = *(undefined2 *)(param_1 + 0x10c);
  local_1c = (undefined2)*(undefined4 *)(iVar2 + 100);
  iVar2 = FUN_80015fe8();
  iVar1 = FUN_80015fe8();
  FUN_8001bf10(iVar2 * 4 + 0x818U | 0x1a000000,
               (int)(((uint)*(ushort *)(param_1 + 0x76) + iVar1 * -4) * 0x10000) >> 0x10,
               &DAT_8009db44,&local_20);
  iVar2 = FUN_80015fe8();
  iVar1 = FUN_80015fe8();
  FUN_8001bf10(iVar2 * 4 + 0x818U | 0x1a000000,
               (int)(((uint)*(ushort *)(param_1 + 0x76) - (iVar1 * 4 + -0x400)) * 0x10000) >> 0x10,
               &DAT_8009db44,&local_20);
  iVar2 = FUN_80015fe8();
  iVar1 = FUN_80015fe8();
  FUN_8001bf10(iVar2 * 4 + 0xc00U | 0x1a000000,
               (int)(((uint)*(ushort *)(param_1 + 0x76) - (iVar1 * 4 + -0x800)) * 0x10000) >> 0x10,
               &DAT_8009db44,&local_20);
  iVar2 = FUN_80015fe8();
  iVar1 = FUN_80015fe8();
  FUN_8001bf10(iVar2 * 4 + 0xc00U | 0x1a000000,
               (int)(((uint)*(ushort *)(param_1 + 0x76) - (iVar1 * 4 + -0xc00)) * 0x10000) >> 0x10,
               &DAT_8009db44,&local_20);
  return;
}


