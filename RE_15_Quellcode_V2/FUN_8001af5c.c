void FUN_8001af5c(undefined2 param_1,undefined2 param_2,undefined2 param_3,undefined2 param_4,
                 int param_5,undefined4 param_6)

{
  u_short uVar1;
  POLY_FT4 *p;
  
  p = (POLY_FT4 *)(param_5 + 0x10);
  *(undefined4 *)(param_5 + 0x14) = param_6;
  SetPolyFT4(p);
  SetSemiTrans(p,1);
  *(undefined2 *)(param_5 + 0x26) = 0x5f;
  uVar1 = GetClut(0x110,0x1f7);
  *(u_short *)(param_5 + 0x1e) = uVar1;
  *(undefined1 *)(param_5 + 0x1d) = 0xe1;
  *(undefined1 *)(param_5 + 0x25) = 0xe1;
  *(undefined1 *)(param_5 + 0x1c) = 1;
  *(undefined1 *)(param_5 + 0x24) = 0x1a;
  *(undefined1 *)(param_5 + 0x2c) = 1;
  *(undefined1 *)(param_5 + 0x2d) = 0xfe;
  *(undefined1 *)(param_5 + 0x34) = 0x1a;
  *(undefined1 *)(param_5 + 0x35) = 0xfe;
  *(undefined2 *)(param_5 + 8) = param_1;
  *(undefined2 *)(param_5 + 10) = param_2;
  *(undefined2 *)(param_5 + 0xc) = param_3;
  *(undefined2 *)(param_5 + 0xe) = param_4;
  FUN_8004ee38(param_5 + 0x38,p,0x28);
  return;
}
