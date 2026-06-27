int sub03(void) {

	Evt_next();
	nop();
	switch(26) {
	case 0:
		Sce_bgm_control(0, 0, 1, 61, 41);
		Sce_bgm_control(0, 0, 2, 61, 65);
		Sce_bgm_control(0, 0, 3, 51, 65);
	break;
	case 256:
		Sce_bgm_control(0, 0, 1, 71, 31);
		Sce_bgm_control(0, 0, 2, 61, 65);
		Sce_bgm_control(0, 0, 3, 51, 65);
	break;
	case 512:
		Sce_bgm_control(0, 0, 1, 81, 51);
		Sce_bgm_control(0, 0, 2, 61, 65);
		Sce_bgm_control(0, 0, 3, 51, 65);
	break;
	case 768:
		Sce_bgm_control(0, 0, 1, 71, 91);
		Sce_bgm_control(0, 0, 2, 61, 65);
		Sce_bgm_control(0, 0, 3, 51, 65);
	break;
	case 1024:
		Sce_bgm_control(0, 0, 1, 61, 101);
		Sce_bgm_control(0, 0, 2, 61, 65);
		Sce_bgm_control(0, 0, 3, 51, 65);
	break;
	}
	while(Ck(1, 11, 0)) {
		Evt_next();
		nop();
	}
	goto(0x00006AFF);
	return 0;
}