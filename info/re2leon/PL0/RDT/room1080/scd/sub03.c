int sub03(void) {

	Evt_next();
	nop();
	while(Ck(5, 5, 1)) {
		Evt_next();
		nop();
	}
	if(Ck(4, 0, 0)) {
		switch(26) {
		case 0:
			Sce_bgm_control(0, 0, 1, 73, 49);
			Sce_bgm_control(0, 0, 2, 58, 64);
			Sce_bgm_control(0, 0, 3, 30, 35);
			Sce_bgm_control(0, 0, 4, 10, 50);
		break;
		case 256:
			Sce_bgm_control(0, 0, 1, 93, 49);
			Sce_bgm_control(0, 0, 2, 50, 80);
			Sce_bgm_control(0, 0, 3, 30, 100);
			Sce_bgm_control(0, 0, 4, 15, 30);
		break;
		case 512:
			Sce_bgm_control(0, 0, 1, 102, 70);
			Sce_bgm_control(0, 0, 2, 60, 80);
			Sce_bgm_control(0, 0, 3, 30, 64);
			Sce_bgm_control(0, 0, 4, 10, 3);
		break;
		case 768:
			Sce_bgm_control(0, 0, 1, 110, 64);
			Sce_bgm_control(0, 0, 2, 60, 80);
			Sce_bgm_control(0, 0, 3, 30, 127);
			Sce_bgm_control(0, 0, 4, 10, 3);
		break;
		case 1024:
			Sce_bgm_control(0, 0, 1, 111, 125);
			Sce_bgm_control(0, 0, 2, 70, 80);
			Sce_bgm_control(0, 0, 3, 30, 34);
			Sce_bgm_control(0, 0, 4, 10, 3);
		break;
		case 1280:
			Sce_bgm_control(0, 0, 1, 98, 77);
			Sce_bgm_control(0, 0, 2, 60, 80);
			Sce_bgm_control(0, 0, 3, 30, 34);
			Sce_bgm_control(0, 0, 4, 10, 3);
		break;
		case 1536:
			Sce_bgm_control(0, 0, 1, 127, 85);
			Sce_bgm_control(0, 0, 2, 60, 80);
			Sce_bgm_control(0, 0, 3, 30, 34);
			Sce_bgm_control(0, 0, 4, 10, 3);
		break;
		}
	} else {
		Sce_bgm_control(0, 0, 1, 1, 0);
		Sce_bgm_control(0, 0, 4, 1, 0);
		switch(26) {
		case 2304:
			Sce_bgm_control(0, 0, 2, 58, 64);
			Sce_bgm_control(0, 0, 3, 30, 35);
		break;
		case 2560:
			Sce_bgm_control(0, 0, 2, 50, 80);
			Sce_bgm_control(0, 0, 3, 30, 100);
		break;
		case 2816:
			Sce_bgm_control(0, 0, 2, 60, 80);
			Sce_bgm_control(0, 0, 3, 30, 64);
		break;
		case 3072:
			Sce_bgm_control(0, 0, 2, 60, 80);
			Sce_bgm_control(0, 0, 3, 30, 127);
		break;
		case 3328:
			Sce_bgm_control(0, 0, 2, 70, 80);
			Sce_bgm_control(0, 0, 3, 30, 34);
		break;
		case 3584:
			Sce_bgm_control(0, 0, 2, 75, 80);
			Sce_bgm_control(0, 0, 3, 30, 34);
		break;
		case 3840:
			Sce_bgm_control(0, 0, 2, 60, 80);
			Sce_bgm_control(0, 0, 3, 30, 34);
		break;
		}
		nop();
		nop();
		do {
			Evt_next();
			nop();
		} while(Ck(1, 11, 0));
		goto(0x000062FE);
		return 0;
}