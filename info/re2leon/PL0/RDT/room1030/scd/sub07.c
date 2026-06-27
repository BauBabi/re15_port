int sub07(void) {

switch(26) {
case 0:
Sce_bgm_control(0, 0, 1, 1, 65);
Sce_bgm_control(0, 0, 2, 58, 65);
Sce_bgm_control(0, 0, 3, 85, 65);
break;
case 256:
case 512:
case 768:
case 1024:
case 1280:
case 1536:
Sce_bgm_control(0, 0, 3, 60, 65);
break;
case 1792:
Sce_bgm_control(0, 0, 3, 74, 65);
break;
case 2048:
Sce_bgm_control(0, 0, 3, 24, 65);
break;
case 2304:
Sce_bgm_control(0, 0, 3, 24, 65);
break;
case 2560:
Sce_bgm_control(0, 0, 1, 1, 65);
Sce_bgm_control(0, 0, 2, 58, 65);
Sce_bgm_control(0, 0, 3, 24, 65);
break;
case 2816:
Sce_bgm_control(0, 0, 3, 24, 65);
break;
case 3072:
Sce_bgm_control(0, 0, 3, 24, 65);
break;
case 3328:
Sce_bgm_control(0, 0, 3, 24, 65);
break;
case 3584:
Sce_bgm_control(0, 0, 3, 24, 65);
break;
case 3840:
Sce_bgm_control(0, 0, 3, 24, 65);
break;
}
do {
Evt_next();
nop();
} while(Ck(1, 11, 0));
goto(0x00001EFF);
return 0;
}