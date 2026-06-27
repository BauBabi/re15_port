void FUN_8002a1b8(void)

{
  DAT_800aca34 = DAT_800aca34 ^ 1;
  SetDefDrawEnv((DRAWENV *)0x14,0,-(uint)(DAT_800aca34 != 0) & 0xf0,(uint)uRam0000007a,0xf0);
  SetDefDispEnv((DISPENV *)0x0,0,-(uint)(DAT_800aca34 == 0) & 0xf0,(uint)uRam0000007a,0xf0);
  uRam0000002a = 0;
  uRam00000011 = uRam00000078;
  uRam0000002c = 1;
  FUN_80061fc0(0);
  PutDrawEnv((DRAWENV *)0x14);
  PutDispEnv((DISPENV *)0x0);
  return;
}
