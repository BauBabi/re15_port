int sub04(void) {

Evt_next();
nop();
switch(26) {
case 0:
Sce_bgm_control(0, 0, 1, 120, 51);
Sce_bgm_control(0, 0, 2, 71, 65);
break;
case 256:
Sce_bgm_control(0, 0, 1, 106, 86);
Sce_bgm_control(0, 0, 2, 71, 65);
break;
case 512:
Sce_bgm_control(0, 0, 1, 96, 33);
Sce_bgm_control(0, 0, 2, 71, 65);
break;
case 768:
Sce_bgm_control(0, 0, 1, 106, 71);
Sce_bgm_control(0, 0, 2, 71, 65);
break;
case 1024:
Sce_bgm_control(0, 0, 1, 81, 128);
Sce_bgm_control(0, 0, 2, 71, 65);
break;
case 1280:
Sce_bgm_control(0, 0, 1, 51, 128);
Sce_bgm_control(0, 0, 2, 61, 65);
break;
case 1536:
Sce_bgm_control(0, 0, 1, 31, 83);
Sce_bgm_control(0, 0, 2, 51, 65);
break;
case 1792:
Sce_bgm_control(0, 0, 1, 26, 43);
Sce_bgm_control(0, 0, 2, 51, 65);
break;
case 2048:
Sce_bgm_control(0, 0, 1, 1, 65);
Sce_bgm_control(0, 0, 2, 41, 65);
break;
}
do {
Evt_next();
nop();
} while(Ck(1, 11, 0));
goto(0x000039FF);
return 0;
}