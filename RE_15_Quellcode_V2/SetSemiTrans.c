void SetSemiTrans(void *p,int abe)

{
  if (abe != 0) {
    PRIM_OBJ_2FC();
    return;
  }
  *(byte *)((int)p + 7) = *(byte *)((int)p + 7) & 0xfd;
  return;
}
