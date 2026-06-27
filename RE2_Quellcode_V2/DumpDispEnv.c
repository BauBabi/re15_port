/* DumpDispEnv @ 0x8008fe34  (Ghidra headless, raw MIPS overlay) */

void DumpDispEnv(DISPENV *env)

{
  (*(code *)PTR_printf_800b26fc)
            ("disp   (%3d,%3d)-(%d,%d)\n",(int)(env->disp).x,(int)(env->disp).y,(int)(env->disp).w,
             (int)(env->disp).h);
  (*(code *)PTR_printf_800b26fc)
            ("screen (%3d,%3d)-(%d,%d)\n",(int)(env->screen).x,(int)(env->screen).y,
             (int)(env->screen).w,(int)(env->screen).h);
  (*(code *)PTR_printf_800b26fc)("isinter %d\n",env->isinter);
  (*(code *)PTR_printf_800b26fc)("isrgb24 %d\n",env->isrgb24);
  return;
}


