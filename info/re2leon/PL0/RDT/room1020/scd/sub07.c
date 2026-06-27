int sub07(void) {

Work_set(4, 0);
nop();
Member_set(15, 0);
Speed_set(4, -20225);
Speed_set(10, -2305);
// unknown opcode (8A) found at 0x00000016
// unknown opcode (8B) found at 0x0000001C
// unknown opcode (8B) found at 0x00000022
for(n = 0; n < 768; n++) {
Add_speed();
Add_aspeed();
}
Evt_next();
nop();
Se_on(2, 10, 1, 4, 0, 0, 0, 0);
for(n = 0; n < 1024; n++) {
Add_speed();
Add_aspeed();
Evt_next();
nop();
}
for(n = 0; n < 1792; n++) {
Add_speed();
Add_aspeed();
Evt_next();
nop();
}
Member_set(15, 8);
Evt_next();
nop();
Speed_set(4, 15360);
Speed_set(10, -4865);
for(n = 0; n < 1536; n++) {
Add_speed();
Add_aspeed();
Evt_next();
nop();
}
return 0;
}