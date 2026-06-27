/* FUN_8003a034 @ 0x8003a034  (Ghidra headless, raw MIPS overlay) */

void FUN_8003a034(undefined2 param_1,undefined2 param_2,undefined2 param_3,undefined2 param_4)

{
  int iVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  
  iVar1 = DAT_800ea23c;
  iVar4 = 3;
  iVar2 = DAT_800ea23c + (uint)DAT_800ce5e0 * 0x40 + 0x5360;
  iVar3 = iVar2 + 0x30;
  do {
    *(undefined4 *)(iVar3 + 4) = 0xffffff;
    iVar4 = iVar4 + -1;
    iVar3 = iVar3 + -0x10;
  } while (-1 < iVar4);
  iVar3 = 3;
  *(undefined2 *)(iVar2 + 8) = param_1;
  *(undefined2 *)(iVar2 + 0xc) = param_3;
  *(undefined2 *)(iVar2 + 0x18) = param_1;
  *(undefined2 *)(iVar2 + 0x1c) = param_3;
  *(undefined2 *)(iVar2 + 0x28) = param_1;
  *(undefined2 *)(iVar2 + 0x2c) = param_1;
  *(undefined2 *)(iVar2 + 0x38) = param_3;
  *(undefined2 *)(iVar2 + 0x3c) = param_3;
  *(undefined2 *)(iVar2 + 10) = param_2;
  *(undefined2 *)(iVar2 + 0xe) = param_2;
  *(undefined2 *)(iVar2 + 0x1a) = param_4;
  *(undefined2 *)(iVar2 + 0x1e) = param_4;
  *(undefined2 *)(iVar2 + 0x2a) = param_2;
  *(undefined2 *)(iVar2 + 0x2e) = param_4;
  *(undefined2 *)(iVar2 + 0x3a) = param_2;
  *(undefined2 *)(iVar2 + 0x3e) = param_4;
  do {
    iVar2 = iVar3 * 0x10;
    SetLineF2((LINE_F2 *)(iVar1 + (uint)DAT_800ce5e0 * 0x40 + 0x5360 + iVar2));
    iVar3 = iVar3 + -1;
    AddPrim(&DAT_800cc22c + (uint)DAT_800ce5e0 * 0x1000,
            (void *)(iVar1 + (uint)DAT_800ce5e0 * 0x40 + 0x5360 + iVar2));
  } while (-1 < iVar3);
  return;
}


