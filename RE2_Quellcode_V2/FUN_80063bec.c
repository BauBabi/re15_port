/* FUN_80063bec @ 0x80063bec  (Ghidra headless, raw MIPS overlay) */

void FUN_80063bec(int param_1,undefined4 param_2,undefined4 param_3)

{
  ushort uVar1;
  char cVar2;
  ushort uVar3;
  ushort uVar4;
  
  FUN_80065518();
  cVar2 = DAT_800a8673 + -1;
  if (DAT_800a8673 == '\0') {
    uVar1 = *(ushort *)(param_1 + 0x10e);
    *(ushort *)(param_1 + 0x10e) = uVar1 & 0xff00;
    uVar3 = FUN_80065890(param_1,3000);
    uVar4 = *(ushort *)(param_1 + 0x21c);
    *(ushort *)(param_1 + 0x10e) = *(ushort *)(param_1 + 0x10e) | uVar3;
    *(ushort *)(param_1 + 0x21c) = uVar4 & 0x7fff;
    if ((uVar1 & 0xf) != (*(ushort *)(param_1 + 0x10e) & 0xf)) {
      uVar4 = uVar4 & 0x7ffc;
      *(ushort *)(param_1 + 0x21c) = uVar4;
      *(ushort *)(param_1 + 0x21c) =
           uVar4 | (byte)(&DAT_800a89cc)[*(ushort *)(param_1 + 0x10e) & 0xf] | 0x8000;
    }
    cVar2 = '\x0f';
  }
  DAT_800a8673 = cVar2;
  (*(code *)(&PTR_LAB_800a89a0)[*(ushort *)(param_1 + 0x10e) & 0xf])(param_1,param_2,param_3);
  return;
}


