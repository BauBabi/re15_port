/* DivideGT3 @ 0x8008ebf4  (Ghidra headless, raw MIPS overlay) */

undefined4
DivideGT3(uint param_1,uint param_2,uint param_3,undefined4 *param_4,undefined4 *param_5,
         undefined4 *param_6,undefined4 *param_7,undefined4 *param_8,undefined4 *param_9,
         undefined4 param_10,undefined4 param_11,int param_12)

{
  uint uVar1;
  uint *puVar2;
  long lVar3;
  uint uVar4;
  uint in_t0;
  uint uVar5;
  uint in_t1;
  uint uVar6;
  long lStack_38;
  long lStack_34;
  long alStack_30 [2];
  
  *(SVECTOR **)(param_12 + 0xa8) = (SVECTOR *)(param_12 + 0x18);
  *(SVECTOR **)(param_12 + 0xac) = (SVECTOR *)(param_12 + 0x30);
  *(SVECTOR **)(param_12 + 0xb0) = (SVECTOR *)(param_12 + 0x48);
  uVar1 = param_1 + 3 & 3;
  uVar6 = param_1 & 3;
  uVar5 = (*(int *)((param_1 + 3) - uVar1) << (3 - uVar1) * 8 |
          in_t0 & 0xffffffffU >> (uVar1 + 1) * 8) & -1 << (4 - uVar6) * 8 |
          *(uint *)(param_1 - uVar6) >> uVar6 * 8;
  uVar1 = param_1 + 7 & 3;
  uVar6 = param_1 + 4 & 3;
  uVar6 = (*(int *)((param_1 + 7) - uVar1) << (3 - uVar1) * 8 |
          in_t1 & 0xffffffffU >> (uVar1 + 1) * 8) & -1 << (4 - uVar6) * 8 |
          *(uint *)((param_1 + 4) - uVar6) >> uVar6 * 8;
  uVar1 = param_12 + 0x1bU & 3;
  puVar2 = (uint *)((param_12 + 0x1bU) - uVar1);
  *puVar2 = *puVar2 & -1 << (uVar1 + 1) * 8 | uVar5 >> (3 - uVar1) * 8;
  uVar1 = param_12 + 0x18U & 3;
  puVar2 = (uint *)((param_12 + 0x18U) - uVar1);
  *puVar2 = *puVar2 & 0xffffffffU >> (4 - uVar1) * 8 | uVar5 << uVar1 * 8;
  uVar1 = param_12 + 0x1fU & 3;
  puVar2 = (uint *)((param_12 + 0x1fU) - uVar1);
  *puVar2 = *puVar2 & -1 << (uVar1 + 1) * 8 | uVar6 >> (3 - uVar1) * 8;
  uVar1 = param_12 + 0x1cU & 3;
  puVar2 = (uint *)((param_12 + 0x1cU) - uVar1);
  *puVar2 = *puVar2 & 0xffffffffU >> (4 - uVar1) * 8 | uVar6 << uVar1 * 8;
  uVar1 = param_2 + 3 & 3;
  uVar6 = param_2 & 3;
  uVar4 = (*(int *)((param_2 + 3) - uVar1) << (3 - uVar1) * 8 |
          param_1 & 0xffffffffU >> (uVar1 + 1) * 8) & -1 << (4 - uVar6) * 8 |
          *(uint *)(param_2 - uVar6) >> uVar6 * 8;
  uVar1 = param_2 + 7 & 3;
  uVar6 = param_2 + 4 & 3;
  uVar6 = (*(int *)((param_2 + 7) - uVar1) << (3 - uVar1) * 8 |
          uVar5 & 0xffffffffU >> (uVar1 + 1) * 8) & -1 << (4 - uVar6) * 8 |
          *(uint *)((param_2 + 4) - uVar6) >> uVar6 * 8;
  uVar1 = param_12 + 0x33U & 3;
  puVar2 = (uint *)((param_12 + 0x33U) - uVar1);
  *puVar2 = *puVar2 & -1 << (uVar1 + 1) * 8 | uVar4 >> (3 - uVar1) * 8;
  uVar1 = param_12 + 0x30U & 3;
  puVar2 = (uint *)((param_12 + 0x30U) - uVar1);
  *puVar2 = *puVar2 & 0xffffffffU >> (4 - uVar1) * 8 | uVar4 << uVar1 * 8;
  uVar1 = param_12 + 0x37U & 3;
  puVar2 = (uint *)((param_12 + 0x37U) - uVar1);
  *puVar2 = *puVar2 & -1 << (uVar1 + 1) * 8 | uVar6 >> (3 - uVar1) * 8;
  uVar1 = param_12 + 0x34U & 3;
  puVar2 = (uint *)((param_12 + 0x34U) - uVar1);
  *puVar2 = *puVar2 & 0xffffffffU >> (4 - uVar1) * 8 | uVar6 << uVar1 * 8;
  uVar1 = param_3 + 3 & 3;
  uVar6 = param_3 & 3;
  uVar4 = (*(int *)((param_3 + 3) - uVar1) << (3 - uVar1) * 8 |
          uVar4 & 0xffffffffU >> (uVar1 + 1) * 8) & -1 << (4 - uVar6) * 8 |
          *(uint *)(param_3 - uVar6) >> uVar6 * 8;
  uVar1 = param_3 + 7 & 3;
  uVar6 = param_3 + 4 & 3;
  uVar6 = (*(int *)((param_3 + 7) - uVar1) << (3 - uVar1) * 8 |
          param_2 & 0xffffffffU >> (uVar1 + 1) * 8) & -1 << (4 - uVar6) * 8 |
          *(uint *)((param_3 + 4) - uVar6) >> uVar6 * 8;
  uVar1 = param_12 + 0x4bU & 3;
  puVar2 = (uint *)((param_12 + 0x4bU) - uVar1);
  *puVar2 = *puVar2 & -1 << (uVar1 + 1) * 8 | uVar4 >> (3 - uVar1) * 8;
  uVar1 = param_12 + 0x48U & 3;
  puVar2 = (uint *)((param_12 + 0x48U) - uVar1);
  *puVar2 = *puVar2 & 0xffffffffU >> (4 - uVar1) * 8 | uVar4 << uVar1 * 8;
  uVar1 = param_12 + 0x4fU & 3;
  puVar2 = (uint *)((param_12 + 0x4fU) - uVar1);
  *puVar2 = *puVar2 & -1 << (uVar1 + 1) * 8 | uVar6 >> (3 - uVar1) * 8;
  uVar1 = param_12 + 0x4cU & 3;
  puVar2 = (uint *)((param_12 + 0x4cU) - uVar1);
  *puVar2 = *puVar2 & 0xffffffffU >> (4 - uVar1) * 8 | uVar6 << uVar1 * 8;
  lVar3 = RotAverageNclip3((SVECTOR *)(param_12 + 0x18),(SVECTOR *)(param_12 + 0x30),
                           (SVECTOR *)(param_12 + 0x48),(long *)(param_12 + 0x28),
                           (long *)(param_12 + 0x40),(long *)(param_12 + 0x58),&lStack_38,&lStack_34
                           ,alStack_30);
  if (0 < lVar3) {
    uVar4 = FUN_8008edcc(param_12 + 0x2c,param_12 + 0x44,param_12 + 0x5c);
    *(undefined4 *)(param_12 + 0x14) = param_11;
    uVar1 = (int)param_7 + 3U & 3;
    uVar6 = (uint)param_7 & 3;
    uVar6 = (*(int *)(((int)param_7 + 3U) - uVar1) << (3 - uVar1) * 8 |
            uVar4 & 0xffffffffU >> (uVar1 + 1) * 8) & -1 << (4 - uVar6) * 8 |
            *(uint *)((int)param_7 - uVar6) >> uVar6 * 8;
    uVar1 = param_12 + 0x13U & 3;
    puVar2 = (uint *)((param_12 + 0x13U) - uVar1);
    *puVar2 = *puVar2 & -1 << (uVar1 + 1) * 8 | uVar6 >> (3 - uVar1) * 8;
    uVar1 = param_12 + 0x10U & 3;
    puVar2 = (uint *)((param_12 + 0x10U) - uVar1);
    *puVar2 = *puVar2 & 0xffffffffU >> (4 - uVar1) * 8 | uVar6 << uVar1 * 8;
    *(undefined2 *)(param_12 + 0xc) = *(undefined2 *)((int)param_4 + 2);
    *(undefined2 *)(param_12 + 0xe) = *(undefined2 *)((int)param_5 + 2);
    *(undefined4 *)(param_12 + 0x20) = *param_4;
    *(undefined4 *)(param_12 + 0x38) = *param_5;
    *(undefined4 *)(param_12 + 0x50) = *param_6;
    *(undefined4 *)(param_12 + 0x24) = *param_7;
    *(undefined4 *)(param_12 + 0x3c) = *param_8;
    *(undefined4 *)(param_12 + 0x54) = *param_9;
    param_10 = RCpolyGT3A(param_10,param_12,0,param_12 + 0x60);
  }
  return param_10;
}


