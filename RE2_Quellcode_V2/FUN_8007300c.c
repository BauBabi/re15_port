/* FUN_8007300c @ 0x8007300c  (Ghidra headless, raw MIPS overlay) */

void * FUN_8007300c(void *param_1,uint param_2,undefined4 param_3,uint param_4)

{
  short sVar1;
  u_short uVar2;
  uint uVar3;
  u_short *puVar4;
  int iVar5;
  int iVar6;
  void *p;
  char cVar7;
  short sVar8;
  
  uVar3 = (uint)DAT_800ce5e0;
  sVar8 = 0;
  if ((param_4 & 0x7f) == 4) {
    param_1 = (void *)((int)param_1 + 0xa0);
  }
  else {
    sVar1 = (short)((uint)param_3 >> 0x10);
    if (param_2 != 0xff) {
      DAT_800c3bd8 = (char)(param_4 >> 0x10);
      DAT_800c3bdb = (char)(param_2 / 100);
      DAT_800c3bda = (char)((param_2 % 100) / 10);
      DAT_800c3bd9 = (char)(param_2 % 100) + DAT_800c3bda * -10;
      if ((param_4 & 0x10) == 0) {
        puVar4 = (u_short *)((int)param_1 + 0xe);
        iVar5 = 4;
        do {
          iVar6 = iVar5 + -1;
          if (((DAT_800c3bdb != '\0') || ((iVar6 != 3 && ((DAT_800c3bda != '\0' || (iVar6 != 2))))))
             && ((DAT_800c3bd8 != '\0' || (iVar6 != 0)))) {
            uVar2 = sVar1 + sVar8;
            sVar8 = sVar8 + 7;
            puVar4[-3] = uVar2;
            puVar4[-2] = (u_short)param_3;
            cVar7 = (&UNK_800c3bd7)[iVar5];
            *(undefined1 *)((int)puVar4 + -1) = 0x1a;
            *(char *)(puVar4 + -1) = cVar7 << 3;
            uVar2 = GetClut(0x100,(param_4 & 0xf) + 0x1ec);
            *puVar4 = uVar2;
            AddPrim(&UNK_800cc1f4 + uVar3 * 0x20,param_1);
          }
          puVar4 = puVar4 + 0x14;
          param_1 = (void *)((int)param_1 + 0x28);
          iVar5 = iVar6;
        } while (iVar6 != 0);
        return param_1;
      }
    }
    iVar5 = 2;
    puVar4 = (u_short *)((int)param_1 + 0xe);
    cVar7 = 'P';
    do {
      p = param_1;
      cVar7 = cVar7 + '\b';
      iVar5 = iVar5 + -1;
      puVar4[-3] = sVar1 + sVar8;
      puVar4[-2] = (u_short)param_3;
      *(char *)(puVar4 + -1) = cVar7;
      *(undefined1 *)((int)puVar4 + -1) = 0x1a;
      uVar2 = GetClut(0x100,(param_4 & 0xf) + 0x1ec);
      *puVar4 = uVar2;
      AddPrim(&UNK_800cc1f4 + uVar3 * 0x20,p);
      sVar8 = sVar8 + 7;
      puVar4 = puVar4 + 0x14;
      param_1 = (void *)((int)p + 0x28);
    } while (iVar5 != 0);
    param_1 = (void *)((int)p + 0x78);
  }
  return param_1;
}


