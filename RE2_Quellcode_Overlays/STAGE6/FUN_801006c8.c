/* FUN_801006c8 @ 0x801006c8  (Ghidra headless overlay RE) */

/* WARNING: Control flow encountered bad instruction data */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801006c8(void)

{
  undefined1 uVar1;
  ushort uVar2;
  ushort uVar3;
  ushort uVar4;
  int iVar5;
  short sVar6;
  int iVar7;
  ushort *puVar8;
  byte *pbVar9;
  int iVar10;
  byte *pbVar11;
  char cVar12;
  char cVar13;
  
  iVar7 = (int)_DAT_8011bcda;
  iVar10 = (uint)DAT_800ce5e0 * 0x500 + -0x7fee4300;
  pbVar9 = _DAT_8011bcf0 + 1;
  puVar8 = (ushort *)((uint)DAT_800ce5e0 * 0x500 + -0x7fee42ea);
  pbVar11 = _DAT_8011bcf0;
  do {
    iVar7 = iVar7 + -1;
    if ((*pbVar11 & 1) != 0) {
      uVar3 = *(ushort *)(pbVar9 + 3);
      uVar4 = *(ushort *)(pbVar9 + 5);
      iVar5 = (uint)pbVar9[2] * 10;
      uVar2 = uVar3 + *(byte *)(iVar5 + -0x7fee4ae4);
      sVar6 = uVar4 + *(byte *)(iVar5 + -0x7fee4ae3);
      cVar12 = *(char *)(iVar5 + -0x7fee4ae8) + *(char *)(iVar5 + -0x7fee4ae6);
      cVar13 = *(char *)(iVar5 + -0x7fee4ae7) + *(char *)(iVar5 + -0x7fee4ae5);
      func_0x8008fa80(iVar10);
      puVar8[-0xffffffff00000009] = 0x8080;
      puVar8[-0xffffffff00000008] = 0x2c80;
      puVar8[-7] = uVar3;
      puVar8[-6] = uVar4;
      *(undefined1 *)(puVar8 + -5) = *(undefined1 *)(iVar5 + -0x7fee4ae8);
      uVar1 = *(undefined1 *)(iVar5 + -0x7fee4ae7);
      puVar8[-2] = uVar4;
      *(char *)(puVar8 + -1) = cVar12;
      puVar8[-3] = uVar2;
      *(undefined1 *)((int)puVar8 + -9) = uVar1;
      *(undefined1 *)((int)puVar8 + -1) = *(undefined1 *)(iVar5 + -0x7fee4ae7);
      if ((*pbVar9 & 0x10) != 0) {
        uVar3 = func_0x8008f828(((uint)*(byte *)(iVar5 + -0x7fee4aeb) + (uint)DAT_8011bce0) * 0x10,
                                *(undefined2 *)(iVar5 + -0x7fee4aea));
        puVar8[-4] = uVar3;
                    /* WARNING: Bad instruction - Truncating control flow here */
        halt_baddata();
      }
      uVar4 = func_0x8008f828((uint)*(byte *)(iVar5 + -0x7fee4aeb) << 4,
                              *(undefined2 *)(iVar5 + -0x7fee4aea));
      puVar8[-4] = uVar4;
      *puVar8 = *(byte *)(iVar5 + -0x7fee4aec) & 0xf;
      *(ushort *)(iVar10 + 0x18) = uVar3;
      *(short *)(iVar10 + 0x1a) = sVar6;
      uVar1 = *(undefined1 *)(iVar5 + -0x7fee4ae8);
      *(char *)(iVar10 + 0x1d) = cVar13;
      *(ushort *)(iVar10 + 0x20) = uVar2;
      puVar8 = puVar8 + 0x14;
      *(short *)(iVar10 + 0x22) = sVar6;
      *(char *)(iVar10 + 0x24) = cVar12;
      *(char *)(iVar10 + 0x25) = cVar13;
      *(undefined1 *)(iVar10 + 0x1c) = uVar1;
      iVar10 = iVar10 + 0x28;
      func_0x8008f918((uint)DAT_800ce5e0 * 0xc0 + -0x7ff1d52c);
    }
    pbVar9 = pbVar9 + 8;
    pbVar11 = pbVar11 + 8;
  } while (iVar7 != 0);
  return;
}


