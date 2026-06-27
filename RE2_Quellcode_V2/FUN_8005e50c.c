/* FUN_8005e50c @ 0x8005e50c  (Ghidra headless, raw MIPS overlay) */

void FUN_8005e50c(int param_1)

{
  undefined4 uVar1;
  undefined4 *puVar2;
  int iVar3;
  int iVar4;
  int iVar5;
  
  if ((*(ushort *)(param_1 + 0x10e) & 0x200) != 0) {
    iVar5 = 7;
    iVar4 = 0xc;
    do {
      iVar3 = *(int *)(param_1 + 0x198) + (uint)(byte)(&UNK_800a83a8)[iVar4] * 0xac;
      puVar2 = (undefined4 *)(*(int *)(param_1 + 0x198) + (uint)(byte)(&UNK_800a83a9)[iVar4] * 0xac)
      ;
      if (*(short *)(iVar3 + 0x84) == 0) {
        *(int *)(iVar3 + 0x70) = *(int *)(iVar3 + 0x70) + 0x10101;
        puVar2[0x1c] = puVar2[0x1c] + -0x10101;
        if ((*(uint *)(iVar3 + 0x70) & 0xffffff) == 0x404040) {
          *(undefined2 *)(iVar3 + 0x84) = 1;
          uVar1 = *(undefined4 *)(iVar3 + 8);
          *(undefined4 *)(iVar3 + 8) = puVar2[2];
          puVar2[2] = uVar1;
          uVar1 = *(undefined4 *)(iVar3 + 0x10);
          *(undefined4 *)(iVar3 + 0x10) = puVar2[4];
          puVar2[4] = uVar1;
          uVar1 = *(undefined4 *)(iVar3 + 0xc);
          *(undefined4 *)(iVar3 + 0xc) = puVar2[3];
          puVar2[3] = uVar1;
          uVar1 = *(undefined4 *)(iVar3 + 0x14);
          *(undefined4 *)(iVar3 + 0x14) = puVar2[5];
          puVar2[5] = uVar1;
        }
      }
      if (*(short *)(iVar3 + 0x84) == 1) {
        *(int *)(iVar3 + 0x70) = *(int *)(iVar3 + 0x70) + -0x10101;
        puVar2[0x1c] = puVar2[0x1c] + 0x10101;
        if ((*(uint *)(iVar3 + 0x70) & 0xffffff) == 0) {
          *(undefined2 *)(iVar3 + 0x84) = 2;
          *(ushort *)(param_1 + 0x10e) = *(ushort *)(param_1 + 0x10e) & 0xfdff;
          uVar1 = *(undefined4 *)(iVar3 + 8);
          *(undefined4 *)(iVar3 + 8) = puVar2[2];
          puVar2[2] = uVar1;
          uVar1 = *(undefined4 *)(iVar3 + 0x10);
          *(undefined4 *)(iVar3 + 0x10) = puVar2[4];
          puVar2[4] = uVar1;
          uVar1 = *(undefined4 *)(iVar3 + 0xc);
          *(undefined4 *)(iVar3 + 0xc) = puVar2[3];
          puVar2[3] = uVar1;
          uVar1 = *(undefined4 *)(iVar3 + 0x14);
          *(undefined4 *)(iVar3 + 0x14) = puVar2[5];
          puVar2[5] = uVar1;
          uVar1 = *(undefined4 *)(iVar3 + 0x70);
          *(undefined4 *)(iVar3 + 0x70) = puVar2[0x1c];
          puVar2[0x1c] = uVar1;
          *(uint *)(iVar3 + 4) = *(uint *)(iVar3 + 4) & 0xfffffffd;
          *puVar2 = 0;
        }
      }
      iVar5 = iVar5 + -1;
      iVar4 = iVar4 + -2;
    } while (iVar5 != 0);
  }
  return;
}


