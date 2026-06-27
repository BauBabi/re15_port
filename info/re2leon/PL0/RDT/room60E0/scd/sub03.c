int sub03(void) {

Message_on(0, 512, 0xFFFF);
Evt_next();
nop();
if(Ck(11, 31, 0)) {
Aot_on(0);
}
return 0;
}