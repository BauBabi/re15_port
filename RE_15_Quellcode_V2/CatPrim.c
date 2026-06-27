void CatPrim(void *p0,void *p1)

{
                    /* WARNING: Load size is inaccurate */
  *(uint *)p0 = *p0 & 0xff000000 | (uint)p1 & 0xffffff;
  return;
}
