int sub07(void) {

	Sce_bgm_control(0, 1, 1, 1, 64);
	Evt_next();
	nop();
	switch(26) {
	case 0:
		Sce_bgm_control(0, 0, 1, 50, 64);
	break;
	case 256:
		Sce_bgm_control(0, 0, 1, 70, 90);
	break;
	case 512:
		Sce_bgm_control(0, 0, 1, 60, 100);
	break;
	case 768:
		Sce_bgm_control(0, 0, 1, 70, 55);
	break;
	case 1024:
		Sce_bgm_control(0, 0, 1, 90, 55);
	break;
	case 1280:
		Sce_bgm_control(0, 0, 1, 70, 64);
	break;
	case 1536:
		Sce_bgm_control(0, 0, 1, 70, 80);
	break;
	case 1792:
		Sce_bgm_control(0, 0, 1, 110, 80);
	break;
	case 2048:
		Sce_bgm_control(0, 0, 1, 80, 40);
	break;
	case 2304:
		Sce_bgm_control(0, 0, 1, 80, 64);
	break;
	case 2560:
		Sce_bgm_control(0, 0, 1, 70, 40);
	break;
	case 2816:
		Sce_bgm_control(0, 0, 1, 60, 100);
	break;
	case 3072:
		Sce_bgm_control(0, 0, 1, 80, 40);
	break;
	case 3328:
		Sce_bgm_control(0, 0, 1, 70, 40);
	break;
	case 3584:
		Sce_bgm_control(0, 0, 1, 70, 40);
	break;
	case 3840:
		Sce_bgm_control(0, 0, 1, 70, 40);
	break;
	}
	do {
		Evt_next();
		nop();
	} while(Ck(1, 11, 0));
	goto(0x00000DFF);
	return 0;
}