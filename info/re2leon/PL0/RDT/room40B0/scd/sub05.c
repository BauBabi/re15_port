int sub05(void) {

	Sce_bgm_control(0, 1, 1, 100, 64);
	while(Ck(5, 1, 1)) {
		switch(26) {
		case 0:
			Sce_bgm_control(0, 0, 1, 80, 64);
		break;
		case 256:
			Sce_bgm_control(0, 0, 1, 60, 80);
		break;
		case 512:
			Sce_bgm_control(0, 0, 1, 50, 64);
		break;
		case 768:
			Sce_bgm_control(0, 0, 1, 70, 64);
		break;
		}
		do {
			Evt_next();
			nop();
		} while(Ck(1, 11, 0));
	}
	return 0;
}