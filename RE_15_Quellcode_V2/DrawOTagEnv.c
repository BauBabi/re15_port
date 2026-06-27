void DrawOTagEnv(uint param_1,DRAWENV *param_2)

{
  if (1 < DAT_8007e352) {
    printf("DrawOTagEnv(%08x,&08x)...\n",param_1,param_2);
  }
  SetDrawEnv(&param_2->dr_env,param_2);
  (param_2->dr_env).tag = (param_2->dr_env).tag & 0xff000000 | param_1 & 0xffffff;
  _addque2(_cwc,&param_2->dr_env,0x40,0);
  memcpy("",(uchar *)param_2,0x5c);
  return;
}
