void FUN_8001db28(void)

{
  byte *pbVar1;
  undefined1 *puVar2;
  u_short uVar3;
  undefined2 uVar4;
  int iVar5;
  int iVar6;
  short sVar7;
  short sVar8;
  undefined4 uVar9;
  uint uVar10;
  undefined2 *puVar11;
  uint unaff_s4;
  RECT RStack_28;
  undefined2 auStack_20 [4];
  
  switch(DAT_80072d3b) {
  case 1:
    RStack_28.x = 0x3f8;
    RStack_28.y = 0;
    RStack_28.w = 1;
    RStack_28.h = 1;
    DAT_800aca40 = DAT_800aca40 | 0xff000000;
    DAT_8008f640 = DAT_800ac77c;
    DAT_800ac77c = DAT_800ac77c + 0x140;
    uVar10 = 0;
    ClearImage(&RStack_28,'\0','\0','\0');
    auStack_20[0] = 0xffff;
    RStack_28.x = 0;
    RStack_28.y = 0x1e9;
    RStack_28.w = 1;
    RStack_28.h = 1;
    LoadImage(&RStack_28,(u_long *)auStack_20);
    DrawSync(0);
    do {
      iVar5 = uVar10 + DAT_8008f640;
      *(undefined2 *)(iVar5 + 0xc) = 0x70;
      *(undefined2 *)(iVar5 + 0x14) = 0x70;
      *(undefined2 *)(iVar5 + 0x1c) = 0x70;
      *(undefined2 *)(iVar5 + 0x24) = 0x70;
      uVar3 = GetClut(0,0x1e9);
      iVar6 = uVar10 + DAT_8008f640;
      *(u_short *)(iVar6 + 0xe) = uVar3;
      iVar5 = GetGraphType();
      uVar4 = 0x20f;
      if (iVar5 != 1) {
        iVar5 = GetGraphType();
        uVar4 = 0x8f;
        if (iVar5 == 2) {
          uVar4 = 0x20f;
        }
      }
      iVar5 = DAT_8008f640;
      *(undefined2 *)(iVar6 + 0x16) = uVar4;
      SetPolyFT4((POLY_FT4 *)(uVar10 + iVar5));
      SetSemiTrans((void *)(uVar10 + DAT_8008f640),1);
      uVar10 = uVar10 + 0x28;
    } while (uVar10 < 0x50);
    FUN_80029a98(1,&LAB_8001e404);
    DAT_80072d3b = 2;
    DAT_8008f630 = 0;
    DAT_8008f634 = 0x800;
    break;
  case 2:
    FUN_8001e1c8(DAT_8008f630,DAT_8008f634,0,0);
    DAT_8008f630 = DAT_8008f630 + 1;
    DAT_8008f634 = DAT_8008f634 + 0x80;
    if (0x10 < DAT_8008f630) {
      DAT_80072d3b = 3;
      DAT_8008f630 = 0;
      DAT_8008f634 = 0;
    }
    goto LAB_8001e170;
  case 3:
    AddPrim(&DAT_800aa6b4 + (uint)DAT_800aca34 * 0x20,
            (void *)((uint)DAT_800aca34 * 0x28 + DAT_8008f640));
    if (DAT_800b29a4 == 0) {
      DAT_80072d3b = 4;
    }
    break;
  case 4:
    iVar5 = rcos((uint)DAT_8008f634);
    iVar5 = iVar5 * 0x24;
    uVar10 = 0;
    if (iVar5 < 0) {
      iVar5 = iVar5 + 0xfff;
    }
    sVar7 = (short)(iVar5 >> 0xc);
    sVar8 = sVar7 + 0x6c;
    sVar7 = 0x6c - sVar7;
    DAT_8008f634 = DAT_8008f634 + 0x100;
    iVar5 = DAT_8008f640;
    do {
      *(short *)(iVar5 + 10) = sVar8;
      *(short *)(iVar5 + 0x12) = sVar8;
      *(short *)(iVar5 + 0x1a) = sVar7;
      *(short *)(iVar5 + 0x22) = sVar7;
      uVar10 = uVar10 + 1;
      iVar5 = iVar5 + 0x28;
    } while (uVar10 < 2);
    if (DAT_8008f630 == 4) {
      uVar10 = 0;
      iVar5 = 0;
      do {
        iVar6 = iVar5 + DAT_8008f640;
        *(undefined2 *)(iVar6 + 0x14) = 0x70;
        *(undefined2 *)(iVar6 + 0x1c) = 0x4800;
        *(undefined2 *)(iVar6 + 0xc) = 0;
        *(undefined2 *)(iVar6 + 0x24) = 0x4870;
        *(undefined1 *)(iVar6 + 6) = 0x80;
        *(undefined1 *)(iVar6 + 5) = 0x80;
        *(undefined1 *)(iVar6 + 4) = 0x80;
        uVar10 = uVar10 + 1;
        SetSemiTrans((void *)(iVar5 + DAT_8008f640),0);
        iVar5 = iVar5 + 0x28;
        _DAT_800aca4c = 0x90f;
        FUN_8004ee78(DAT_800ac77c);
      } while (uVar10 < 2);
    }
    AddPrim(&DAT_800aa6b4 + (uint)DAT_800aca34 * 0x20,
            (void *)((uint)DAT_800aca34 * 0x28 + DAT_8008f640));
    DAT_8008f630 = DAT_8008f630 + 1;
    if (DAT_8008f630 < 9) {
      return;
    }
  case 5:
    DAT_8008f62c = FUN_8004df2c();
    DAT_800aca40 = unaff_s4 & 0xffff;
    if (DAT_800b8523 - 0xe < 6) {
      if (DAT_8008f62c != 0xff) {
        uVar9 = 0;
        DAT_800b8523 = DAT_800afbb6;
        goto LAB_8001dfe0;
      }
      FUN_80027e68(0,0x100,1,0xff000000);
      DAT_8008f62c = 0xff;
    }
    else {
      if ((int)((uint)DAT_8008f62c << 0x18) < 0) {
        uVar9 = 1;
      }
      else {
        uVar9 = 0;
        DAT_800b8523 = DAT_800afbb6;
      }
LAB_8001dfe0:
      FUN_80027e68(0,0x100,uVar9,0xff000000);
    }
    DAT_80072d3b = 6;
    break;
  case 6:
    AddPrim(&DAT_800aa6b4 + (uint)DAT_800aca34 * 0x20,
            (void *)((uint)DAT_800aca34 * 0x28 + DAT_8008f640));
    if ((DAT_800b8520 & 0x80) != 0) {
      return;
    }
  case 7:
    puVar2 = DAT_800aca30;
    if (((char)DAT_8008f62c < '\0') || ((DAT_800b8520 & 1) != 0)) {
      DAT_8008f630 = 0;
      DAT_8008f634 = 0;
      DAT_80072d3b = 8;
    }
    else {
      pbVar1 = DAT_800aca30 + 1;
      *DAT_800aca30 = 0;
      if ((*pbVar1 & 0x80) == 0) {
        puVar11 = (undefined2 *)(puVar2 + 0xc);
      }
      else {
        puVar11 = (undefined2 *)(puVar2 + 0x14);
      }
      DAT_800b0fd6 = *puVar11;
      FUN_8004dc4c(DAT_800afbb6,puVar11[1],DAT_800ac77c + 0x21a0);
      FUN_8004ef90(&DAT_800b1078,puVar11[2]);
      puVar11[3] = 0;
      DAT_80072d3b = 0;
    }
    break;
  case 8:
    FUN_8001e1c8(0x10 - DAT_8008f630,0,1,0x80);
    DAT_8008f630 = DAT_8008f630 + 1;
    DAT_8008f634 = DAT_8008f634 + 0x80;
    if (0x10 < DAT_8008f630) {
      DAT_80072d3b = 0;
    }
LAB_8001e170:
    AddPrim(&DAT_800aa6b4 + (uint)DAT_800aca34 * 0x20,
            (void *)((uint)DAT_800aca34 * 0x28 + DAT_8008f640));
  }
  return;
}
