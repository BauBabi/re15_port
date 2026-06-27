u_short GetTPage(int tp,int abr,int x,int y)

{
  u_short uVar1;
  int iVar2;
  
  iVar2 = GetGraphType();
  if (iVar2 != 1) {
    iVar2 = GetGraphType();
    if (iVar2 != 2) {
      return (ushort)((tp & 3U) << 7) | (ushort)((abr & 3U) << 5) | (ushort)((int)(y & 0x100U) >> 4)
             | (ushort)((int)(x & 0x3ffU) >> 6) | (ushort)((y & 0x200U) << 2);
    }
  }
  uVar1 = PRIM_OBJ_A4();
  return uVar1;
}
