void SetShadeTex(void *p,int tge)

{
  if (tge != 0) {
    PRIM_OBJ_324();
    return;
  }
  *(byte *)((int)p + 7) = *(byte *)((int)p + 7) & 0xfe;
  return;
}
