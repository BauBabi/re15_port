int IsEndPrim(void *p)

{
                    /* WARNING: Load size is inaccurate */
  return (uint)((*p & 0xffffff) == 0xffffff);
}
