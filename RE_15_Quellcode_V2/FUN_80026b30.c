void FUN_80026b30(void)

{
  undefined *p;
  void *ot;
  
  ot = (void *)FUN_800c74f0();
  p = PTR_DAT_80010168;
  *(undefined4 *)PTR_DAT_80010168 = 0x4000000;
                    /* Probable PsyQ macro: setDrawTPage() if setlen(p, 1), setDrawMode() if
                       setlen(p, 2). */
  *(undefined4 *)(p + 4) = 0xe1000000;
  *(undefined4 *)(p + 8) = 0x62202020;
  *(undefined4 *)(p + 0xc) = 0x340012;
  *(undefined4 *)(p + 0x10) = 0x66011e;
  AddPrim(ot,p);
  PTR_DAT_80010168 = p + 0x14;
  return;
}
