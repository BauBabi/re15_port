int DrawSync(int mode)

{
  int iVar1;
  
  if (1 < DAT_8007e352) {
    printf("DrawSync(%d)...\n",mode);
  }
  iVar1 = _sync(mode);
  return iVar1;
}
