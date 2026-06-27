u_long DrawSyncCallback(func *func)

{
  func *pfVar1;
  
  if (1 < DAT_8007e352) {
    printf("DrawSyncCallback(%08x)...\n",func);
  }
  pfVar1 = DAT_8007e35c;
  DAT_8007e35c = func;
  return (u_long)pfVar1;
}
