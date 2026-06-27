DRAWENV * PutDrawEnv(DRAWENV *env)

{
  if (1 < DAT_8007e352) {
    printf("PutDrawEnv(%08x)...\n",env);
  }
  SetDrawEnv(&env->dr_env,env);
  (env->dr_env).tag = (env->dr_env).tag | 0xffffff;
  _addque2(_cwc,&env->dr_env,0x40,0);
  memcpy("",(uchar *)env,0x5c);
  return env;
}
