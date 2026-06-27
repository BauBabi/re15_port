int sub29(void) {

Aot_reset(18, 0, 0, 0, 0, 0x00);
if(Ck(5, 3, 0)) {
Message_on(0, 4608, 0xFFFF);
Set(5, 3, 1);
} else {
if(Ck(8, 15, 0)) {
Aot_reset(11, 2, 49, 1792, 1024, 0xF00);
Aot_on(11);
} else {
Message_on(0, 4608, 0xFFFF);
nop();
nop();
nop();
nop();
Aot_reset(18, 5, 49, 65280, 6173, 0x00);
return 0;
}