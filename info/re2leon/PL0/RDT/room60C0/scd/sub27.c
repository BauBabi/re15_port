int sub27(void) {

Work_set(4, 2);
nop();
Super_reset(0, 64, 253, 0, 253, 0, 252);
Pos_set(0, -17356, 0, 29891);
Speed_set(0, 7680);
Speed_set(6, 1280);
for(n = 0; n < 7680; n++) {
Add_speed();
Evt_next();
}
Speed_set(0, 7680);
Speed_set(6, -1025);
for(n = 0; n < 2560; n++) {
Add_speed();
Add_aspeed();
Evt_next();
nop();
}
Set(5, 3, 1);
return 0;
}