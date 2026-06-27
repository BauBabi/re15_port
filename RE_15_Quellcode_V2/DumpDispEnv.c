void DumpDispEnv(DISPENV *env)

{
  printf("disp   (%3d,%3d)-(%d,%d)\n",(int)(env->disp).x,(int)(env->disp).y,(int)(env->disp).w,
         (int)(env->disp).h);
  printf("screen (%3d,%3d)-(%d,%d)\n",(int)(env->screen).x,(int)(env->screen).y,(int)(env->screen).w
         ,(int)(env->screen).h);
  printf("isinter %d\n",(uint)env->isinter);
  printf("isrgb24 %d\n",(uint)env->isrgb24);
  return;
}
