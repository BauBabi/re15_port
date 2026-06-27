int sub04(void) {

Message_on(0, 256, 0xFFFF);
Evt_next();
nop();
if(Ck(11, 31, 0)) {
Aot_on(1);
}
return 0;
}