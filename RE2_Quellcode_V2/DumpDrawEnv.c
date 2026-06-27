/* DumpDrawEnv @ 0x8008fd20  (Ghidra headless, raw MIPS overlay) */

void DumpDrawEnv(DRAWENV *env)

{
  ushort uVar1;
  
  (*(code *)PTR_printf_800b26fc)
            ("clip (%3d,%3d)-(%d,%d)\n",(int)(env->clip).x,(int)(env->clip).y,(int)(env->clip).w,
             (int)(env->clip).h);
  (*(code *)PTR_printf_800b26fc)("ofs  (%3d,%3d)\n",(int)env->ofs[0],(int)env->ofs[1]);
  (*(code *)PTR_printf_800b26fc)
            ("tw   (%d,%d)-(%d,%d)\n",(int)(env->tw).x,(int)(env->tw).y,(int)(env->tw).w,
             (int)(env->tw).h);
  (*(code *)PTR_printf_800b26fc)("dtd   %d\n",env->dtd);
  (*(code *)PTR_printf_800b26fc)("dfe   %d\n",env->dfe);
  uVar1 = env->tpage;
  (*(code *)PTR_printf_800b26fc)
            ("tpage: (%d,%d,%d,%d)\n",uVar1 >> 7 & 3,uVar1 >> 5 & 3,(uVar1 & 0x1f) << 6,
             (uVar1 & 0x10) * 0x10 + (uVar1 >> 2 & 0x200));
  return;
}


