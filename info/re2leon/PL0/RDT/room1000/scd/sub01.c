int sub01(void) {

switch(26) {
case 0:
case 256:
if(Ck(5, 7, 0)) {
Set(5, 7, 1);
Evt_exec(255, 0x1805);
}
if(Ck(5, 5, 0)) {
Set(5, 5, 1);
Evt_exec(255, 0x1806);
}
break;
case 512:
if(Ck(5, 6, 0)) {
Set(5, 6, 1);
Evt_exec(255, 0x1807);
}
break;
}
return 0;
}