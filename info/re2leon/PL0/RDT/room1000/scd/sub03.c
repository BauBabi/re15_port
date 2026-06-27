int sub03(void) {

switch(26) {
case 768:
if(Ck(5, 12, 0)) {
Ck(5, 14, 1)
Set(5, 14, 0);
Set(5, 12, 1);
if(Ck(0, 25, 0)) {
goto Sub08();
} else {
nop();
nop();
}
break;
case 1280:
if(Ck(5, 14, 0)) {
Set(5, 14, 1);
Set(5, 12, 0);
if(Ck(0, 25, 0)) {
goto Sub09();
} else {
goto Sub11();
nop();
nop();
}
break;
}
Evt_next();
nop();
goto(0x00009EFF);
return 0;
}