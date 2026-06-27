void TermPrim(void *p)

{
                    /* WARNING: Load size is inaccurate */
  *(uint *)p = *p | 0xffffff;
  return;
}
