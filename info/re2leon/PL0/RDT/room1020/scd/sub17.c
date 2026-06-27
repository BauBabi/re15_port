int sub17(void) {

Evt_next();
nop();
switch(26) {
case 0:
Sce_bgm_control(0, 0, 1, 1, 65);
Sce_bgm_control(0, 0, 2, 61, 65);
Sce_bgm_control(0, 0, 3, 51, 65);
break;
case 256:
Sce_bgm_control(0, 0, 1, 1, 65);
Sce_bgm_control(0, 0, 2, 61, 65);
Sce_bgm_control(0, 0, 3, 51, 65);
break;
case 512:
Sce_bgm_control(0, 0, 1, 1, 65);
Sce_bgm_control(0, 0, 2, 61, 65);
Sce_bgm_control(0, 0, 3, 51, 65);
break;
case 768:
Sce_bgm_control(0, 0, 1, 1, 65);
Sce_bgm_control(0, 0, 2, 61, 65);
Sce_bgm_control(0, 0, 3, 51, 65);
break;
case 1024:
Sce_bgm_control(0, 0, 1, 1, 65);
Sce_bgm_control(0, 0, 2, 71, 65);
Sce_bgm_control(0, 0, 3, 51, 65);
break;
case 1280:
Sce_bgm_control(0, 0, 1, 1, 65);
Sce_bgm_control(0, 0, 2, 71, 65);
Sce_bgm_control(0, 0, 3, 61, 65);
break;
case 1536:
Sce_bgm_control(0, 0, 1, 1, 65);
Sce_bgm_control(0, 0, 2, 61, 65);
Sce_bgm_control(0, 0, 3, 51, 65);
break;
case 1792:
Sce_bgm_control(0, 0, 1, 1, 65);
Sce_bgm_control(0, 0, 2, 61, 65);
Sce_bgm_control(0, 0, 3, 51, 65);
break;
}
do {
Sleep(256);
} while(Ck(1, 11, 0));
goto(0x00001BFF);
return 0;
}