int sub03(void) {

	switch(26) {
	case 0:
		Sce_bgm_control(0, 0, 1, 20, 98);
		Sce_bgm_control(0, 0, 2, 58, 65);
		Sce_bgm_control(0, 0, 3, 45, 65);
	break;
	case 256:
		Sce_bgm_control(0, 0, 1, 25, 58);
		Sce_bgm_control(0, 0, 2, 50, 65);
		Sce_bgm_control(0, 0, 3, 70, 65);
	break;
	case 512:
		Sce_bgm_control(0, 0, 1, 20, 125);
		Sce_bgm_control(0, 0, 2, 60, 80);
		Sce_bgm_control(0, 0, 3, 65, 64);
	break;
	case 768:
		Sce_bgm_control(0, 0, 1, 20, 125);
		Sce_bgm_control(0, 0, 2, 60, 65);
		Sce_bgm_control(0, 0, 3, 65, 65);
	break;
	case 1024:
		Sce_bgm_control(0, 0, 1, 20, 125);
		Sce_bgm_control(0, 0, 2, 70, 65);
		Sce_bgm_control(0, 0, 3, 55, 65);
	break;
	case 1280:
		Sce_bgm_control(0, 0, 1, 20, 125);
		Sce_bgm_control(0, 0, 2, 61, 65);
		Sce_bgm_control(0, 0, 3, 55, 65);
	break;
	case 1536:
		Sce_bgm_control(0, 0, 1, 100, 70);
		Sce_bgm_control(0, 0, 2, 51, 65);
		Sce_bgm_control(0, 0, 3, 54, 65);
	break;
	case 1792:
		Sce_bgm_control(0, 0, 1, 76, 125);
		Sce_bgm_control(0, 0, 2, 51, 65);
		Sce_bgm_control(0, 0, 3, 54, 65);
	break;
	case 2048:
		Sce_bgm_control(0, 0, 1, 40, 125);
		Sce_bgm_control(0, 0, 2, 51, 65);
		Sce_bgm_control(0, 0, 3, 52, 65);
	break;
	case 2304:
		Sce_bgm_control(0, 0, 1, 20, 125);
		Sce_bgm_control(0, 0, 2, 51, 65);
		Sce_bgm_control(0, 0, 3, 40, 65);
	break;
	case 2560:
		Sce_bgm_control(0, 0, 1, 20, 125);
		Sce_bgm_control(0, 0, 2, 51, 65);
		Sce_bgm_control(0, 0, 3, 40, 65);
	break;
	}
	do {
		Sleep(256);
	} while(Ck(1, 11, 0));
	goto(0x0000CEFE);
	return 0;
}