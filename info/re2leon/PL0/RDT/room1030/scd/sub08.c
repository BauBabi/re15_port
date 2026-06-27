int sub08(void) {

if(Ck(5, 21, 0)) {
Work_set(3, 0);
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
Calc(16 || 128)
Member_set2(7, 16);
nop();
Member_copy(16, 7);
nop();
Calc(16 || 64)
Member_set2(7, 16);
nop();
}
if(Ck(5, 22, 0)) {
Work_set(3, 5);
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
Calc(16 || 128)
Member_set2(7, 16);
nop();
Member_copy(16, 7);
nop();
Calc(16 || 64)
Member_set2(7, 16);
nop();
}
return 0;
}