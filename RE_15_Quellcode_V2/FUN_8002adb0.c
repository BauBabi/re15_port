u_char FUN_8002adb0(void)

{
  u_char local_10;
  u_char local_f [7];
  
  local_10 = 0x90;
  CdControlB('\x0e',&local_10,local_f);
  FUN_80061fc0(3);
  FUN_8002ae38();
  return local_f[0];
}
