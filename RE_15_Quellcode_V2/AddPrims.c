void AddPrims(void *ot,void *p0,void *p1)

{
                    /* WARNING: Load size is inaccurate */
                    /* WARNING: Load size is inaccurate */
                    /* Probable PsyQ macro: addPrim(). */
  *(uint *)p1 = *p1 & 0xff000000 | *ot & 0xffffff;
                    /* WARNING: Load size is inaccurate */
  *(uint *)ot = *ot & 0xff000000 | (uint)p0 & 0xffffff;
  return;
}
