int sub12(void) {

Work_set(3, 1);
nop();
Member_set(12, 131);
Member_copy(16, 0);
nop();
Calc(16 || 4)
Member_set2(0, 16);
nop();
Member_copy(16, 1);
nop();
Calc(16 || 2048)
Member_set2(1, 16);
nop();
Member_copy(16, 7);
nop();
Calc(16 || 64)
Member_set2(7, 16);
nop();
Member_copy(16, 7);
nop();
Calc(16 || 128)
Member_set2(7, 16);
nop();
Work_set(3, 2);
nop();
Member_set(12, 131);
Member_copy(16, 0);
nop();
Calc(16 || 4)
Member_set2(0, 16);
nop();
Member_copy(16, 1);
nop();
Calc(16 || 2048)
Member_set2(1, 16);
nop();
Member_copy(16, 7);
nop();
Calc(16 || 64)
Member_set2(7, 16);
nop();
Member_copy(16, 7);
nop();
Calc(16 || 128)
Member_set2(7, 16);
nop();
Work_set(3, 3);
nop();
Member_set(12, 131);
Member_copy(16, 0);
nop();
Calc(16 || 4)
Member_set2(0, 16);
nop();
Member_copy(16, 1);
nop();
Calc(16 || 2048)
Member_set2(1, 16);
nop();
Member_copy(16, 7);
nop();
Calc(16 || 64)
Member_set2(7, 16);
nop();
Member_copy(16, 7);
nop();
Calc(16 || 128)
Member_set2(7, 16);
nop();
if(Ck(5, 16, 1)) {
Work_set(3, 4);
nop();
Member_set(12, 131);
Member_copy(16, 0);
nop();
Calc(16 || 4)
Member_set2(0, 16);
nop();
Member_copy(16, 1);
nop();
Calc(16 || 2048)
Member_set2(1, 16);
nop();
Member_copy(16, 7);
nop();
Calc(16 || 64)
Member_set2(7, 16);
nop();
Member_copy(16, 7);
nop();
Calc(16 || 128)
Member_set2(7, 16);
nop();
}
return 0;
}