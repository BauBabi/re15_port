void DumpDrawEnv(DRAWENV *env)

{
  ushort uVar1;
  int iVar2;
  
  printf("clip (%3d,%3d)-(%d,%d)\n",(int)(env->clip).x,(int)(env->clip).y,(int)(env->clip).w,
         (int)(env->clip).h);
  printf("ofs  (%3d,%3d)\n",(int)env->ofs[0],(int)env->ofs[1]);
  printf("tw   (%d,%d)-(%d,%d)\n",(int)(env->tw).x,(int)(env->tw).y,(int)(env->tw).w,
         (int)(env->tw).h);
  printf("dtd   %d\n",(uint)env->dtd);
  printf("dfe   %d\n",(uint)env->dfe);
  iVar2 = GetGraphType();
  if ((iVar2 != 1) && (iVar2 = GetGraphType(), iVar2 != 2)) {
    uVar1 = env->tpage;
    printf("tpage: (%d,%d,%d,%d)\n",uVar1 >> 7 & 3,uVar1 >> 5 & 3,(uVar1 & 0x1f) << 6,
           (uVar1 & 0x10) * 0x10 + (uVar1 >> 2 & 0x200));
    return;
  }
  uVar1 = env->tpage;
  printf("tpage: (%d,%d,%d,%d)\n",uVar1 >> 9 & 3,uVar1 >> 7 & 3,(uVar1 & 0x1f) << 6,
         (uVar1 & 0x60) << 3);
  PRIM_OBJ_7EC();
  return;
}
