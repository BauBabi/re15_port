int sub03(void) {

	switch(26) {
	case 0:
		Sce_bgm_control(1, 0, 1, 50, 65);
	break;
	case 256:
		Sce_bgm_control(1, 0, 1, 58, 72);
	break;
	case 512:
		Sce_bgm_control(1, 0, 1, 68, 72);
	break;
	case 768:
		Sce_bgm_control(1, 0, 1, 90, 40);
	break;
	case 1024:
		Sce_bgm_control(1, 0, 1, 98, 76);
	break;
	case 1280:
		Sce_bgm_control(1, 0, 1, 103, 72);
	break;
	case 1536:
		Sce_bgm_control(1, 0, 1, 121, 65);
	break;
	case 1792:
		Sce_bgm_control(1, 0, 1, 121, 65);
	break;
	case 2048:
		Sce_bgm_control(1, 0, 1, 68, 40);
	break;
	case 2304:
		Sce_bgm_control(1, 0, 1, 109, 31);
	break;
	case 2560:
		Sce_bgm_control(1, 0, 1, 109, 31);
	break;
	}
	return 0;
}