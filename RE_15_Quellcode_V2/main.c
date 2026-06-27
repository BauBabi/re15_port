void main(void)

{
  undefined1 *puVar1;
  uint uVar2;
  uint *puVar3;
  bool bVar4;
  int iVar5;
  uint uVar6;
  RECT RStack_30;
  
  __main();
  bVar4 = false;
  uVar6 = 0;
  FUN_800297c8();
  FUN_80020f8c();
LAB_80020c10:
  while( true ) {
    while( true ) {
      if ((DAT_800aca38 & 0x8000) != 0) {
        DAT_800aca38 = DAT_800aca38 & 0xffff7fff;
        FUN_80029cd8();
      }
      ClearOTagR((u_long *)(&UNK_800aa698 + (uint)DAT_800aca34 * 0x20),8);
      ClearOTagR((u_long *)(&DAT_800aa6d8 + (uint)DAT_800aca34 * 0x1000),0x400);
      ClearOTagR((u_long *)(&DAT_800ac6d8 + (uint)DAT_800aca34 * 0x40),0x10);
      FUN_80030444();
      if ((DAT_800ac760 & 0x90c) != 0x90c) break;
      puVar1 = (undefined1 *)((int)&RStack_30.y + 1);
      uVar2 = (uint)puVar1 & 3;
      puVar3 = (uint *)(puVar1 + -uVar2);
      *puVar3 = *puVar3 & -1 << (uVar2 + 1) * 8 | 0U >> (3 - uVar2) * 8;
      RStack_30.x = 0;
      RStack_30.y = 0;
      puVar1 = (undefined1 *)((int)&RStack_30.h + 1);
      uVar2 = (uint)puVar1 & 3;
      puVar3 = (uint *)(puVar1 + -uVar2);
      *puVar3 = *puVar3 & -1 << (uVar2 + 1) * 8 | 0x1e00140U >> (3 - uVar2) * 8;
      RStack_30.w = 0x140;
      RStack_30.h = 0x1e0;
      ClearImage(&RStack_30,'\0','\0','\0');
      DrawSync(0);
      FUN_80029b48(0);
      FUN_80029b48(1);
      FUN_80029b48(2);
      FUN_80043958();
      DAT_800b5456 = 2;
      FUN_80021eb4();
      ClearOTagR((u_long *)(&UNK_800aa698 + (uint)DAT_800aca34 * 0x20),8);
      ClearOTagR((u_long *)(&DAT_800aa6d8 + (uint)DAT_800aca34 * 0x1000),0x400);
      ClearOTagR((u_long *)(&DAT_800ac6d8 + (uint)DAT_800aca34 * 0x40),0x10);
      FUN_800299a4(0,0);
    }
    if ((DAT_800aca38 & 0x800000) == 0) break;
    if ((DAT_800ac766 & 0x10) != 0) {
      bVar4 = (bool)(bVar4 ^ 1);
    }
    if (!bVar4) break;
    FUN_80061fc0(0);
    if ((DAT_800ac760 & 2) != 0) goto LAB_80020dc8;
    uVar6 = 0;
  }
  goto LAB_80020ddc;
LAB_80020dc8:
  uVar6 = (uVar6 & 7) + 1;
  if (uVar6 == 1) {
LAB_80020ddc:
    FUN_80029690();
    FUN_800298b0();
    if ((DAT_800aca38 & 0x10000) == 0) {
      FUN_80021a0c();
      FUN_80010000();
      FUN_80021880();
      FUN_8002137c();
    }
    else {
      iVar5 = FUN_8002178c(0);
      if (iVar5 != 0) {
        DrawSync(0);
        PutDispEnv((DISPENV *)(&DAT_800b5428 + (uint)DAT_800aca34 * 0x14));
        DAT_800b5458 = 0x14;
        (&DAT_800b5468)[(uint)DAT_800aca34 * 0x10] = 0x10;
        (&DAT_800b5469)[(uint)DAT_800aca34 * 0x10] = 0x10;
        (&DAT_800b546a)[(uint)DAT_800aca34 * 0x10] = 0x10;
      }
      AddPrim(&UNK_800aa698 + (uint)DAT_800aca34 * 0x20,&DAT_800b5464 + (uint)DAT_800aca34 * 0x10);
      AddPrim(&UNK_800aa698 + (uint)DAT_800aca34 * 0x20,&UNK_800b5484 + (uint)DAT_800aca34 * 0xc);
      DrawSync(0);
      FUN_80061fc0(0);
      DrawOTag((u_long *)(&DAT_800aa6b4 + (uint)DAT_800aca34 * 0x20));
      DAT_800b5458 = DAT_800b5458 - 1;
      if ((int)((uint)DAT_800b5458 << 0x10) < 0) {
        DAT_800aca38 = DAT_800aca38 & 0xfffeffff;
      }
    }
  }
  goto LAB_80020c10;
}
