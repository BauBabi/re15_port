int sub03(void) {

	switch(26) {
	case 0:
		Sce_bgm_control(0, 0, 1, 120, 59);
		Sce_bgm_control(0, 0, 2, 71, 65);
	break;
	case 256:
		Sce_bgm_control(0, 0, 1, 101, 25);
		Sce_bgm_control(0, 0, 2, 71, 65);
	break;
	case 512:
		Sce_bgm_control(0, 0, 1, 61, 1);
		Sce_bgm_control(0, 0, 2, 71, 65);
	break;
	case 768:
		Sce_bgm_control(0, 0, 1, 31, 1);
		Sce_bgm_control(0, 0, 2, 71, 65);
	break;
	case 1024:
		Sce_bgm_control(0, 0, 1, 1, 65);
		Sce_bgm_control(0, 0, 2, 71, 65);
	break;
	case 1280:
		Sce_bgm_control(0, 0, 1, 1, 65);
		Sce_bgm_control(0, 0, 2, 51, 65);
	break;
	case 1536:
		Sce_bgm_control(0, 0, 1, 1, 65);
		Sce_bgm_control(0, 0, 2, 41, 65);
	break;
	}
	do {
		Evt_next();
		nop();
	} while(Ck(1, 11, 0));
	goto(0x000062FF);
	return 0;
}