void * NextPrim(void *p)

{
                    /* WARNING: Load size is inaccurate */
                    /* Probable PsyQ macro: nextPrim(), with pattern: *pPrim & 0xffffff |
                       0x80000000. */
  return (void *)(*p & 0xffffff | 0x80000000);
}
