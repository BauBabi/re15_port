int sub03(void) {

if(Ck(5, 5, 1)) {
Ck(6, 62, 1)
Ck(6, 63, 1)
Ck(6, 64, 1)
Ck(6, 65, 1)
Set(5, 5, 0);
Sce_bgm_control(1, 5, 0, 0, 65);
}
if(Ck(5, 4, 0)) {
Ck(8, 15, 1)
Set(5, 4, 1);
Work_set(3, 0);
nop();
Parts_set(0, 11, 2, 0);
}
return 0;
}