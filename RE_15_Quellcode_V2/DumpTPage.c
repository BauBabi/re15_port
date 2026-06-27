void DumpTPage(u_short tpage)

{
  int iVar1;
  
  iVar1 = GetGraphType();
  if (iVar1 != 1) {
    iVar1 = GetGraphType();
    if (iVar1 != 2) {
      printf("tpage: (%d,%d,%d,%d)\n",tpage >> 7 & 3,tpage >> 5 & 3,(tpage & 0x1f) << 6,
             (tpage & 0x10) * 0x10 + (tpage >> 2 & 0x200));
      return;
    }
  }
  printf("tpage: (%d,%d,%d,%d)\n",tpage >> 9 & 3,tpage >> 7 & 3,(tpage & 0x1f) << 6,
         (tpage & 0x60) << 3);
  PRIM_OBJ_19C();
  return;
}
