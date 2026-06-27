/* main @ 0x8002aee0  (Ghidra headless, raw MIPS overlay) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void main(void)

{
  undefined1 *puVar1;
  uint uVar2;
  uint *puVar3;
  ushort uVar4;
  int iVar5;
  RECT RStack_20;
  
  __main();
  FUN_8002b48c();
LAB_8002af20:
  while( true ) {
    if (DAT_800dfd60 != '\0') {
      trap(0x400);
    }
    DAT_800cc228 = (u_long *)(&DAT_800cc1e8 + (uint)DAT_800ce5e0 * 0x20);
    DAT_800ce2b0 = (u_long *)(&UNK_800ce230 + (uint)DAT_800ce5e0 * 0x40);
    DAT_800ce22c = (u_long *)(&DAT_800cc22c + (uint)DAT_800ce5e0 * 0x1000);
    DAT_800dfc10 = &DAT_800dfae0 + (uint)DAT_800ce5e0 * 0x98;
    ClearOTagR(DAT_800cc228,8);
    ClearOTagR(DAT_800ce22c,0x400);
    ClearOTagR(DAT_800ce2b0,0x10);
    if (DAT_800ce2b4 == -1) {
      DAT_800ce2b7 = 0xff;
      DAT_800ce2b6 = 0xff;
      DAT_800ce2d8._3_1_ = 0xff;
      DAT_800ce2d8._2_1_ = 0xff;
    }
    if ((DAT_800cfb74 & 0x1000) == 0) break;
    puVar1 = (undefined1 *)((int)&RStack_20.y + 1);
    uVar2 = (uint)puVar1 & 3;
    puVar3 = (uint *)(puVar1 + -uVar2);
    *puVar3 = *puVar3 & -1 << (uVar2 + 1) * 8 | DAT_800109e8 >> (3 - uVar2) * 8;
    RStack_20.x = (undefined2)DAT_800109e8;
    RStack_20.y = DAT_800109e8._2_2_;
    puVar1 = (undefined1 *)((int)&RStack_20.h + 1);
    uVar2 = (uint)puVar1 & 3;
    puVar3 = (uint *)(puVar1 + -uVar2);
    *puVar3 = *puVar3 & -1 << (uVar2 + 1) * 8 | DAT_800109ec >> (3 - uVar2) * 8;
    RStack_20.w = (undefined2)DAT_800109ec;
    RStack_20.h = DAT_800109ec._2_2_;
    DAT_800ce304 = 0;
    DrawSync(0);
    VSync(0);
    ResetCallback();
    _DAT_800d5300 = 0;
    ClearImage(&RStack_20,'\0','\0','\0');
    FUN_8002bda8(2,0);
    FUN_80032030(0);
    FUN_80032030(1);
    FUN_80032030(2);
    FUN_80059514();
    SsEnd();
    FUN_8007cbc8();
    FUN_80059438();
    FUN_8002c610();
    DAT_800dfd55 = 0;
    ClearOTagR(DAT_800cc228,8);
    ClearOTagR(DAT_800ce22c,0x400);
    ClearOTagR(DAT_800ce2b0,0x10);
    FUN_80031bf0();
    FUN_800778f8();
    FUN_80031e80(0,0);
  }
  do {
    uVar4 = DAT_800dfd5c;
    if (((((((DAT_800cfb74 & 0x40000000) == 0) || (DAT_800dfc1b != '\0')) || (DAT_800df348 != '\0'))
         || ((_DAT_800d5300 != 0 || ((DAT_800cfbdc & 0x1000000) != 0)))) ||
        (((DAT_800cfb74 & 0x2202200) != 0 || ((DAT_800cfbd8 & 0x100010) != 0)))) ||
       ((DAT_800dfd5a == '\0' && (DAT_800ce2b4 != -1)))) {
      DAT_800dfd58 = 0;
      if (DAT_800dfd5a != '\0') break;
    }
    else {
      if (1 < DAT_800dfd58) {
        DAT_800dfd5c = ~CONCAT11(DAT_800ce2b6,DAT_800ce2b7);
        if ((DAT_800dfd5c & 0x400) != 0) {
          DAT_800dfd5c = DAT_800dfd5c | 0x800;
        }
        DAT_800dfd5e = DAT_800dfd5c & (uVar4 ^ DAT_800dfd5c);
        if ((DAT_800dfd5e & 0x800) != 0) {
          FUN_8005ce30(1);
          DAT_800dfd5a = '\0';
        }
        DAT_800cbc21 = 0;
        DAT_800cbc20 = 0;
        VSync(0);
        goto LAB_8002af20;
      }
      if (DAT_800dfd58 == 0) {
        AddPrim(DAT_800cc228 + 1,&DAT_8009dbec + (uint)DAT_800ce5e0 * 0x10);
        AddPrim(DAT_800cc228 + 1,&DAT_800dfb5c + (uint)DAT_800ce5e0 * 0x98);
        FUN_8002fa28(0x87,0x6b,0,0x4000,"Paused");
      }
      DAT_800dfd58 = DAT_800dfd58 + 1;
      if (DAT_800dfd5a != '\0') {
        FUN_8005ce30(0);
        goto LAB_8002b310;
      }
    }
    FUN_80038bbc();
    if ((DAT_800ce300 & 0x800) == 0) break;
    DAT_800dfd5a = '\x01';
    DAT_800dfd5c = (ushort)DAT_800ce2fc;
  } while( true );
  DAT_800dfd5a = '\0';
LAB_8002b310:
  FUN_800778f8();
  FUN_80031bf0();
  FUN_80031d8c();
  DAT_800cfbd8 = DAT_800cfbd8 & 0xffcfffff;
  if ((DAT_800cfb74 & 0x10000) == 0) {
    FUN_8003022c();
    FUN_8002c378();
    FUN_8002b968();
    FUN_80012984();
  }
  else {
    iVar5 = FUN_8002c350(0);
    if (iVar5 != 0) {
      DrawSync(0);
      PutDispEnv((DISPENV *)(DAT_800dfc10 + 0x5c));
      SetDrawMode((DR_MODE *)(&UNK_800dfc48 + (uint)DAT_800ce5e0 * 0xc),1,0,0x40,(RECT *)0x0);
      FUN_8002c2b0(0,0x1f,0x80808,0);
    }
    AddPrim(DAT_800cc228,&DAT_800dfc28 + (uint)DAT_800ce5e0 * 0x10);
    AddPrim(DAT_800cc228,&UNK_800dfc48 + (uint)DAT_800ce5e0 * 0xc);
    DAT_800dfc1c = DAT_800dfc1c - 1;
    if ((int)((uint)DAT_800dfc1c << 0x10) < 0) {
      DAT_800cfb74 = DAT_800cfb74 & 0xfffeffff;
    }
    DrawSync(0);
    VSync(0);
    DrawOTag(DAT_800cc228 + 7);
  }
  goto LAB_8002af20;
}


