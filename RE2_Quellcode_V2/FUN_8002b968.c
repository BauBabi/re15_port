/* FUN_8002b968 @ 0x8002b968  (Ghidra headless, raw MIPS overlay) */

void FUN_8002b968(void)

{
  int y;
  u_long *p;
  undefined4 *puVar1;
  
  DrawSync(0);
  VSync((uint)DAT_800dfc1a);
  FUN_8005c5e4();
  PutDrawEnv(DAT_800dfc10);
  PutDispEnv((DISPENV *)(DAT_800dfc10 + 1));
  if (((DAT_800cfbd8 & 0x80) == 0) && (DAT_800d4820 != DAT_800cfbf2)) {
    DAT_800dfc1b = '\x01';
  }
  if ((DAT_800cfb74 & 0x200) != 0) {
    FUN_80032340();
  }
  if (DAT_800dfd54 == '\0') {
    if (DAT_800dfc1b == '\0') {
      if ((DAT_800d4496 & 1) == 0) {
        p = &DAT_80198000;
        puVar1 = *(undefined4 **)(DAT_800ce324 + 0x44);
      }
      else {
        p = *(u_long **)(DAT_800ce324 + 0x44);
        puVar1 = &DAT_80198000;
      }
      if (DAT_800dfd57 != '\0') {
        p = p + 5;
      }
      if (DAT_8009dc10 != 0) {
        DAT_8009dc16 = DAT_800dfc10->ofs[1];
        DAT_8009dc1a = (short)DAT_8009dc10;
        LoadImage((RECT *)&DAT_8009dc14,p);
      }
      DAT_8009dc1a = DAT_800d4494;
      DAT_8009dc16 = DAT_800dfc10->ofs[1] + (short)DAT_8009dc10;
      LoadImage((RECT *)&DAT_8009dc14,
                puVar1 + ((DAT_8009dc10 + DAT_8009dc0c) - (int)DAT_800d4494) * 0xa0);
      DAT_8009dc1a = (short)DAT_8009dc0c - DAT_800d4494;
      DAT_8009dc16 = DAT_800d4494 + DAT_800dfc10->ofs[1] + (short)DAT_8009dc10;
      LoadImage((RECT *)&DAT_8009dc14,p + DAT_8009dc10 * 0xa0);
      if (DAT_8009dc0c != 0xf0) {
        DAT_8009dc1a = 0xf0 - (short)(DAT_8009dc10 + DAT_8009dc0c);
        DAT_8009dc16 = DAT_800dfc10->ofs[1] + (short)DAT_8009dc10 + (short)DAT_8009dc0c;
        LoadImage((RECT *)&DAT_8009dc14,p + (DAT_8009dc10 + DAT_8009dc0c) * 0xa0);
      }
      goto LAB_8002bd0c;
    }
    FUN_8002bdf4();
    DAT_800cfbd8 = DAT_800cfbd8 | 0x100000;
    if (DAT_800dfaea == 0) {
      SetDefDrawEnv((DRAWENV *)&DAT_800dfae0,0,0xf0,0x140,0xf0);
      SetDefDrawEnv((DRAWENV *)&DAT_800dfb78,0,0,0x140,0xf0);
      SetDefDispEnv((DISPENV *)&DAT_800dfb3c,0,0,0x140,0xf0);
      y = 0xf0;
    }
    else {
      SetDefDrawEnv((DRAWENV *)&DAT_800dfae0,0,0,0x140,0xf0);
      SetDefDrawEnv((DRAWENV *)&DAT_800dfb78,0,0xf0,0x140,0xf0);
      SetDefDispEnv((DISPENV *)&DAT_800dfb3c,0,0xf0,0x140,0xf0);
      y = 0;
    }
    SetDefDispEnv((DISPENV *)&DAT_800dfbd4,0,y,0x140,0xf0);
    DAT_800dfb8f = 1;
    DAT_800dfaf7 = 1;
    DAT_800dfb8e = 0;
    DAT_800dfaf6 = 0;
    if ((DAT_800cfb74 & 0x200) == 0) goto LAB_8002bd60;
  }
  else {
LAB_8002bd0c:
    if (DAT_800dfd54 == '\x02') {
      SetTile((TILE *)(DAT_800dfc10[1].dr_env.code + 3));
      AddPrim((void *)(DAT_800ce2b0 + 0x3c),DAT_800dfc10[1].dr_env.code + 3);
    }
  }
  DrawOTag((u_long *)(DAT_800ce2b0 + 0x3c));
  DrawOTag((u_long *)(DAT_800ce22c + 0xffc));
  DrawOTag((u_long *)(DAT_800cc228 + 0x1c));
LAB_8002bd60:
  DAT_800ce5e0 = DAT_800ce5e0 ^ 1;
  if ((DAT_800cfb74 & 0x200) == 0) {
    DAT_800dfc1b = '\0';
  }
  return;
}


