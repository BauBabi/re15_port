int sub02(void) {

	if(Ck(4, 0, 0)) {
		switch(26) {
		case 0:
			Sce_bgm_control(0, 0, 1, 52, 45);
		break;
		case 256:
			Sce_bgm_control(0, 0, 1, 56, 45);
		break;
		case 512:
			Sce_bgm_control(0, 0, 1, 72, 92);
		break;
		case 768:
			Sce_bgm_control(0, 0, 1, 96, 58);
		break;
		case 1024:
			Sce_bgm_control(0, 0, 1, 127, 66);
		break;
		case 1280:
			Sce_bgm_control(0, 0, 1, 110, 64);
		break;
		}
	}
	do {
		Evt_next();
		nop();
	} while(Ck(1, 11, 0));
	goto(0x000090FF);
	return 0;
}