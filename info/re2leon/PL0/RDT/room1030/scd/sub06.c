int sub06(void) {

switch(26) {
case 1024:
case 1280:
if(Ck(5, 1, 0)) {
Set(5, 1, 1);
Set(5, 3, 0);
goto Sub08();
}
break;
case 256:
case 2560:
case 2816:
if(Ck(5, 2, 0)) {
Set(5, 2, 1);
Set(5, 3, 0);
goto Sub09();
}
if(Ck(5, 17, 0)) {
Set(5, 17, 1);
goto Sub12();
}
break;
case 0:
if(Ck(5, 2, 0)) {
Set(5, 2, 1);
Set(5, 3, 0);
goto Sub09();
}
if(Ck(5, 17, 1)) {
Set(5, 17, 0);
goto Sub11();
}
break;
case 1792:
case 2048:
case 2304:
case 3072:
case 3328:
case 3584:
case 3840:
if(Ck(5, 2, 0)) {
Set(5, 2, 1);
Set(5, 3, 0);
goto Sub09();
}
break;
case 512:
case 768:
case 1536:
if(Ck(5, 3, 0)) {
Set(5, 3, 1);
Set(5, 1, 0);
Set(5, 2, 0);
goto Sub10();
}
break;
}
Evt_next();
nop();
goto(0x000006FF);
return 0;
}