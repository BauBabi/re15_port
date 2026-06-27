/* FUN_8005c5e4 @ 0x8005c5e4  (Ghidra headless, raw MIPS overlay) */

void FUN_8005c5e4(void)

{
  char cVar1;
  int iVar2;
  int iVar3;
  long lVar4;
  uint uVar5;
  short *psVar6;
  char *pcVar7;
  int iVar9;
  uint uVar10;
  undefined2 *puVar11;
  short local_38;
  short local_36 [11];
  char *pcVar8;
  
  uVar10 = 0x18;
  puVar11 = &DAT_800d4cd0;
  psVar6 = &DAT_800d5204;
  pcVar8 = (char *)&DAT_800d5218;
  iVar9 = 0x30;
  do {
    pcVar7 = pcVar8 + -0x20;
    puVar11 = puVar11 + -1;
    uVar10 = uVar10 - 1;
    if (*pcVar7 != '\0') {
      if (psVar6[3] == 0) {
        DAT_800d7598 = psVar6[1];
        DAT_800d759a = psVar6[2];
      }
      else {
        FUN_8005c970(pcVar8 + -0xc);
        if (*(int *)(psVar6 + -4) == 1) {
          DAT_800d7598 = DAT_800d7598 + 10;
          if (0x7f < DAT_800d7598) {
            DAT_800d7598 = 0x7f;
          }
          DAT_800d759a = DAT_800d759a + 10;
          if (0x7f < DAT_800d759a) {
            DAT_800d759a = 0x7f;
          }
        }
      }
      uVar5 = (uint)DAT_800e8768;
      if (uVar5 == 0) {
LAB_8005c8b8:
        *pcVar7 = '\0';
      }
      else {
        DAT_800d7598 = (ushort)((DAT_800d7598 * uVar5) / 100);
        DAT_800d759a = (ushort)((DAT_800d759a * uVar5) / 100);
        if ((byte)(&UNK_800d4c9f)[iVar9] < 0x10) {
          FUN_8007fdc8((int)psVar6[-4],(int)psVar6[-3],(int)psVar6[-2],(int)psVar6[-1],(int)*psVar6,
                       (int)(short)DAT_800d7598,(int)(short)DAT_800d759a);
          *pcVar7 = '\0';
        }
        else {
          if ((psVar6[-4] != 2) || (*(char *)((int)psVar6 + -0xb) != '\0')) {
            FUN_800802b8((&UNK_800d4c9f)[iVar9],(int)psVar6[-4],(int)psVar6[-3],(int)psVar6[-2],
                         (int)psVar6[-1],(int)*psVar6,(int)(short)DAT_800d7598,
                         (int)(short)DAT_800d759a);
            goto LAB_8005c8b8;
          }
          FUN_80081530((&UNK_800d4c9f)[iVar9],&local_38,local_36);
          iVar2 = (local_38 * 0x5a) / 100;
          local_38 = (short)iVar2;
          iVar3 = (local_36[0] * 0x5a) / 100;
          local_36[0] = (short)iVar3;
          FUN_8008156c((&UNK_800d4c9f)[iVar9],iVar2 * 0x10000 >> 0x10,iVar3 * 0x10000 >> 0x10);
          cVar1 = *pcVar7;
          *pcVar7 = cVar1 + '\x01';
          if ((char)(cVar1 + '\x01') == '\x10') {
            *pcVar7 = '\0';
          }
          (&UNK_800d4c9e)[iVar9] = 0;
        }
      }
    }
    if (((uVar10 < 0x16) || (lVar4 = SpuGetKeyStatus(1 << (uVar10 & 0x1f)), lVar4 == 3)) ||
       (lVar4 == 0)) {
      *(undefined1 *)puVar11 = 0;
    }
    psVar6 = psVar6 + -0x10;
    pcVar8 = pcVar7;
    iVar9 = iVar9 + -2;
    if (uVar10 == 0) {
      FUN_8005b3b0();
      return;
    }
  } while( true );
}


