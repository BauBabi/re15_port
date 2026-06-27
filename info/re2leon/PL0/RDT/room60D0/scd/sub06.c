int sub06(void) {

Set(4, 119, 1);
Set(2, 7, 1);
Set(1, 27, 1);
Cut_chg(0x04);
goto Sub04();
Work_set(3, 0);
nop();
Pos_set(0, 30901, 31480, -29520);
Member_set(15, 4);
Work_set(4, 2);
nop();
Pos_set(0, -13650, -18696, 28846);
Work_set(4, 0);
nop();
Pos_set(0, 14515, -30985, 8884);
Evt_next();
nop();
goto Sub12();
Speed_set(1, 3072);
Speed_set(4, 1);
Sleep(2560);
for(n = 0; n < 9216; n++) {
Add_speed();
Evt_next();
}
Sleep(2560);
Work_set(3, 0);
nop();
Speed_set(4, -3841);
while(Member_cmp(15 >= 13315)) {
Add_speed();
Evt_next();
}
Member_set(15, 13315);
Save(4, 2560);
goto Sub07();
goto Sub08();
Evt_exec(255, 0x1809);
Save(4, 2048);
goto Sub07();
goto Sub08();
Save(4, 2816);
goto Sub07();
goto Sub08();
Speed_set(4, 4096);
while(Member_cmp(15 < -31226)) {
Add_speed();
Evt_next();
}
Member_set(15, -31226);
Evt_exec(255, 0x180A);
Save(4, 1280);
goto Sub07();
goto Sub08();
Speed_set(4, -3841);
while(Member_cmp(15 >= 4)) {
Add_speed();
Evt_next();
}
Member_set(15, 4);
Work_set(4, 3);
nop();
Speed_set(1, -2817);
Speed_set(4, 255);
for(n = 0; n < 9216; n++) {
Add_speed();
Evt_next();
}
Wsleep();
Wsleeping();
Sce_Item_lost(77);
Aot_on(1);
Cut_old();
nop();
goto Sub04();
Work_set(3, 0);
nop();
Pos_set(0, 125, -11016, 125);
Work_set(4, 2);
nop();
Pos_set(0, 6265, 4345, 6265);
Work_set(4, 0);
nop();
Pos_set(0, 12405, -30985, 12405);
Work_set(4, 3);
nop();
Pos_set(0, 12405, -30985, 18545);
Evt_next();
Set(1, 27, 0);
Set(2, 7, 0);
nop();
return 0;
}