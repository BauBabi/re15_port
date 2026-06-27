/* FUN_8003a298 @ 0x8003a298  (Ghidra headless, raw MIPS overlay) */

undefined4 FUN_8003a298(int param_1,int param_2,int *param_3)

{
  bool bVar1;
  byte *pbVar2;
  int iVar3;
  byte *pbVar4;
  byte bVar5;
  int iVar6;
  
  pbVar2 = DAT_800ea23c;
  if (*param_3 == 0) {
    if (*(int *)(DAT_800ea23c + 0x2f8) != param_2) {
      *(int *)(DAT_800ea23c + 0x2f8) = param_2;
      *(int *)(pbVar2 + 0x2fc) = param_2;
      pbVar2[0x2d6] = 1;
    }
    bVar5 = pbVar2[0x2d6];
    pbVar2[0x2d6] = bVar5 - 1;
    if ((byte)(bVar5 - 1) == 0) {
      pbVar4 = *(byte **)(pbVar2 + 0x2fc);
      pbVar2[0x2d6] = (byte)param_1;
      bVar5 = *pbVar4;
      bVar1 = false;
      do {
        if (bVar5 == 0) {
          pbVar2[0x2f8] = 0;
          pbVar2[0x2f9] = 0;
          pbVar2[0x2fa] = 0;
          pbVar2[0x2fb] = 0;
          *param_3 = 1;
          return 1;
        }
        bVar5 = *pbVar4;
        if (bVar5 == 0x5d) {
          *pbVar2 = 0;
          pbVar2[1] = pbVar2[1] + 1;
        }
        else if (bVar5 < 0x5e) {
          if (bVar5 == 0x5b) {
            *(byte **)(pbVar2 + 0x2fc) = pbVar4 + 1;
            iVar3 = *(int *)(pbVar2 + 0x2fc);
            pbVar2[0x2d6] = (pbVar4[1] - 0x30) * 'd';
            *(int *)(pbVar2 + 0x2fc) = iVar3 + 1;
            iVar6 = *(int *)(pbVar2 + 0x2fc);
            bVar5 = pbVar2[0x2d6] + (*(char *)(iVar3 + 1) + -0x30) * '\n';
            pbVar2[0x2d6] = bVar5;
            *(int *)(pbVar2 + 0x2fc) = iVar6 + 1;
            pbVar2[0x2d6] = *(char *)(iVar6 + 1) + (bVar5 - 0x30);
            *(int *)(pbVar2 + 0x2fc) = *(int *)(pbVar2 + 0x2fc) + 1;
            return 0;
          }
LAB_8003a454:
          bVar1 = true;
          pbVar2[(uint)*pbVar2 + (uint)pbVar2[1] * 0x1e + 4] = **(byte **)(pbVar2 + 0x2fc);
          pbVar2[(uint)*pbVar2 + (uint)pbVar2[1] * 0x1e + 0x16c] = pbVar2[0x2d7];
          pbVar2[0x2d5] = 0x10;
          *pbVar2 = *pbVar2 + 1;
        }
        else if (bVar5 == 0x5e) {
          *(byte **)(pbVar2 + 0x2fc) = pbVar4 + 1;
          bVar5 = pbVar4[1];
          *(byte **)(pbVar2 + 0x2fc) = pbVar4 + 2;
          *pbVar2 = *pbVar2 - (pbVar4[2] + (bVar5 - 0x30) * '\n' + -0x30);
        }
        else {
          if (bVar5 != 0x7c) goto LAB_8003a454;
          *(byte **)(pbVar2 + 0x2fc) = pbVar4 + 1;
          pbVar2[0x2d7] = pbVar4[1] - 0x30;
        }
        if (0x1d < *pbVar2) {
          *pbVar2 = 0;
          pbVar2[1] = pbVar2[1] + 1;
        }
        if (0xb < pbVar2[1]) {
          pbVar2[1] = pbVar2[1] - 1;
          FUN_8003a5a4(0x2100000);
        }
        FUN_8005ba28(0x2100000,0);
        iVar3 = *(int *)(pbVar2 + 0x2fc);
        pbVar4 = (byte *)(iVar3 + 1);
        *(byte **)(pbVar2 + 0x2fc) = pbVar4;
        if ((param_1 != 0) && (bVar1)) {
          return 0;
        }
        bVar5 = *(byte *)(iVar3 + 1);
      } while( true );
    }
  }
  return 0;
}


