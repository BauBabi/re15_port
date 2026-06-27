int sub11(void) {

if(Ck(5, 4, 1)) {
Work_set(3, 0);
nop();
if(Member_cmp(24 ? 128)) {
Se_on(2, 32, 1, 3, 0, 0, 0, 0);
if(Ck(5, 7, 0)) {
Set(5, 7, 1);
goto Sub15();
} else {
Set(5, 7, 0);
goto Sub16();
nop();
nop();
}
Work_set(3, 0);
nop();
if(Member_cmp(24 ? 64)) {
} else {
Set(5, 4, 0);
nop();
nop();
Member_set(24, 0);
}
if(Ck(5, 5, 1)) {
Work_set(3, 1);
nop();
if(Member_cmp(24 ? 128)) {
Se_on(2, 32, 1, 3, 1, 0, 0, 0);
}
if(Member_cmp(24 ? 64)) {
} else {
Set(5, 5, 0);
nop();
nop();
Member_set(24, 0);
}
Evt_next();
nop();
goto(0x000064FF);
return 0;
}