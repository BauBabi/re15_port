/* FUN_80096dd4 @ 0x80096dd4  (Ghidra headless, raw MIPS overlay) */

undefined4 FUN_80096dd4(int param_1)

{
  byte bVar1;
  undefined1 uVar2;
  byte bVar3;
  int iVar4;
  
  bVar1 = *(byte *)(param_1 + 0x46);
  if (bVar1 == 3) {
    bVar1 = *(byte *)(*(int *)(param_1 + 0x3c) + 4);
    bVar3 = *(byte *)(*(int *)(param_1 + 0x3c) + 5);
    *(undefined1 *)(param_1 + 0x47) = 0;
    *(ushort *)(param_1 + 0xe6) = (ushort)bVar3 + (ushort)bVar1 * 0x100;
  }
  else {
    if (3 < bVar1) {
      if (bVar1 != 4) {
        return 1;
      }
      bVar3 = *(char *)(param_1 + 0x47) + 1;
      bVar1 = *(byte *)(*(int *)(param_1 + 0x3c) + 4);
      *(byte *)(param_1 + 0x47) = bVar3;
      *(uint *)(param_1 + 0xec) = *(int *)(param_1 + 0xec) + 8 + (bVar1 + 3 & 0x1fc);
      if (*(byte *)(param_1 + 0xea) <= bVar3) {
        iVar4 = FUN_80096f20(param_1);
        if (iVar4 < 0x81) {
          *(undefined1 *)(param_1 + 0x46) = 0xff;
          FUN_80096f58(param_1,param_1 + 99);
          *(undefined1 *)(param_1 + 0x46) = 2;
        }
        else {
          *(undefined1 *)(param_1 + 0x46) = 0xfe;
          *(undefined1 *)(param_1 + 0x49) = 2;
        }
      }
      return 0;
    }
    if (bVar1 != 2) {
      return 1;
    }
    *(undefined1 *)(param_1 + 0xe3) = *(undefined1 *)(*(int *)(param_1 + 0x3c) + 3);
    uVar2 = *(undefined1 *)(*(int *)(param_1 + 0x3c) + 4);
    *(undefined2 *)(param_1 + 0xe6) = 0;
    *(undefined1 *)(param_1 + 0xe4) = uVar2;
    *(undefined1 *)(param_1 + 0xe9) = *(undefined1 *)(*(int *)(param_1 + 0x3c) + 5);
    uVar2 = *(undefined1 *)(*(int *)(param_1 + 0x3c) + 6);
    *(undefined4 *)(param_1 + 0xec) = 0;
    *(undefined1 *)(param_1 + 0xea) = uVar2;
  }
  return 1;
}


