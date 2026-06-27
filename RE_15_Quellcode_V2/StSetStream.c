void StSetStream(u_long mode,u_long start_frame,u_long end_frame,func1 *func1,func2 *func2)

{
  FUN_8006549c(1);
  DAT_800bbd88 = 0;
  DAT_800b24b0 = func1;
  DAT_800b223c = mode & 1;
  DAT_800b2b40 = 0;
  DAT_800b266c = 0;
  DAT_800b2208 = 0;
  DAT_800aa5e0 = 0;
  DAT_800b24b4 = func2;
  return;
}
