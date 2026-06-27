/* FUN_800408c8 @ 0x800408c8  (Ghidra headless, raw MIPS overlay) */

void FUN_800408c8(int param_1)

{
  ushort uVar1;
  TIM_IMAGE *pTVar2;
  undefined4 uVar3;
  int iVar4;
  int *piVar5;
  undefined4 *puVar6;
  undefined4 *puVar7;
  int *piVar8;
  uint uVar9;
  char cVar10;
  TIM_IMAGE TStack_48;
  undefined4 *local_30;
  
  puVar7 = DAT_800ce31c;
  piVar8 = DAT_800ce31c + 0x35c;
  local_30 = DAT_800ce31c;
  cVar10 = *(char *)(param_1 + 8);
  if (cVar10 == '\x0f') {
    uVar9 = (uint)((*(ushort *)(param_1 + 0x10e) & 0xfff) != 0);
    uVar1 = DAT_800a6d26;
  }
  else {
    if (((cVar10 == '\b') || (cVar10 == '\n')) && ((*(ushort *)(param_1 + 0x10e) & 0xfff) == 2)) {
      iVar4 = 0x20b;
      goto LAB_800409b0;
    }
    uVar9 = *(ushort *)(param_1 + 0x10e) & 0xfff;
    uVar1 = *(ushort *)(&DAT_800a6d08 + (uint)*(byte *)(param_1 + 8) * 2);
  }
  iVar4 = uVar1 + uVar9;
LAB_800409b0:
  FUN_80012fb8(iVar4,piVar8,0,"WEP DATA");
  piVar5 = (int *)((int)piVar8 + *piVar8);
  cVar10 = *(char *)(param_1 + 0x107);
  uVar9 = *(uint *)((int)piVar8 + piVar5[2] + 8) >> 1;
  *(char *)(param_1 + 0x107) = cVar10 + (char)uVar9;
  *(int *)(param_1 + 0x184) = (int)piVar8 + *piVar5;
  *(int *)(param_1 + 0x180) = (int)piVar8 + piVar5[1];
  OpenTIM((u_long *)((int)piVar8 + piVar5[3]));
  pTVar2 = ReadTIM(&TStack_48);
  pTVar2->prect->x = (ushort)*(byte *)(param_1 + 0x104) * 0x40 + -0x39c;
  pTVar2->prect->y = 0x1e0;
  pTVar2->prect->w = 0x1c;
  pTVar2->prect->h = 0x20;
  puVar6 = puVar7 + 5;
  LoadImage(pTVar2->prect,pTVar2->paddr);
  DrawSync(0);
  pTVar2->crect->x = 0xf0;
  pTVar2->crect->y = *(byte *)(param_1 + 0x105) + 0x1e1;
  pTVar2->crect->w = 0x10;
  LoadImage(pTVar2->crect,pTVar2->caddr);
  DrawSync(0);
  FUN_80076b60(2,(int)piVar8 + piVar5[2],*(undefined1 *)(param_1 + 0x104),
               *(undefined1 *)(param_1 + 0x105));
  iVar4 = (int)piVar8 + piVar5[2] + 0xc;
  uVar3 = DAT_800ce320;
  do {
    puVar6[0x18] = &DAT_8009db44;
    puVar6[0x20] = 0;
    *(undefined2 *)(puVar6 + 9) = 0;
    puVar6[6] = 0;
    *(undefined2 *)((int)puVar6 + 0x26) = 0;
    puVar6[7] = 0;
    *(undefined2 *)(puVar6 + 10) = 0;
    puVar6[8] = 0;
    puVar6[-3] = iVar4;
    puVar6[-1] = iVar4 + 0x1c;
    puVar6[0xd] = 0x1000;
    puVar6[0xe] = 0;
    puVar6[0xf] = 0x1000;
    puVar6[0x10] = 0;
    puVar6[0x11] = 0x1000;
    puVar6[0x12] = 0;
    puVar6[0x13] = 0;
    puVar6[0x14] = 0;
    puVar6[1] = 0x1000;
    puVar6[2] = 0;
    puVar6[3] = 0x1000;
    puVar6[4] = 0;
    puVar6[5] = 0x1000;
    puVar6[-4] = 0;
    puVar6[0x17] = 0x808080;
    puVar6[0x15] = 0;
    *(undefined2 *)(puVar6 + 0x16) = 0;
    *puVar7 = 0;
    *(undefined2 *)(puVar6 + 0x12) = 0;
    *(char *)(puVar6 + 0x19) = cVar10;
    puVar6[-2] = uVar3;
    uVar3 = FUN_8002cbc4(puVar7 + 1,uVar3,1);
    *puVar6 = uVar3;
    uVar3 = FUN_8002cd24(puVar7 + 1,uVar3,1);
    cVar10 = cVar10 + '\x01';
    puVar6 = puVar6 + 0x2b;
    puVar7 = puVar7 + 0x2b;
    uVar9 = uVar9 - 1;
    iVar4 = iVar4 + 0x38;
  } while (uVar9 != 0);
  iVar4 = *(int *)(param_1 + 0x198);
  *(undefined4 *)(iVar4 + 0x76c) = local_30[2];
  *(undefined4 *)(iVar4 + 0x774) = local_30[4];
  *(undefined4 *)(iVar4 + 0x770) = local_30[3];
  *(undefined4 *)(iVar4 + 0x778) = local_30[5];
  (*(code *)(&PTR_LAB_800a6d28)[*(ushort *)(param_1 + 0x10e) & 0xfff])(param_1);
  return;
}


