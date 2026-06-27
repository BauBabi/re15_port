/* DpqColor3 @ 0x8008de84  (Ghidra headless, raw MIPS overlay) */

void DpqColor3(CVECTOR *_2,CVECTOR *_3,CVECTOR *v2,long p,CVECTOR *v3,CVECTOR *v4,CVECTOR *v5)

{
  undefined1 auStackX_0 [16];
  
  gte_ldrgb3(_2,_3,v2);
  gte_ldIR0(p);
  gte_dpct_b();
  gte_strgb3(*(CVECTOR **)((int)register0x00000074 + 0x10),
             *(CVECTOR **)((int)register0x00000074 + 0x14),
             *(CVECTOR **)((int)register0x00000074 + 0x18));
  return;
}


