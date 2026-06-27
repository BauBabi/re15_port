int sub00(void) {

goto Sub02();
if(Ck(5, 18, 0)) {
goto Sub16();
Evt_exec(255, 0x1803);
Evt_exec(255, 0x1805);
Evt_exec(255, 0x1806);
Evt_exec(255, 0x1811);
} else {
Evt_exec(255, 0x1803);
Evt_exec(255, 0x1805);
goto Sub15();
Evt_exec(255, 0x1813);
Evt_exec(255, 0x1812);
Evt_exec(255, 0x1814);
nop();
nop();
Set(13, 5, 0);
Evt_exec(255, 0x1807);
return 0;
}