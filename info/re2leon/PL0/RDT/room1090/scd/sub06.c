int sub06(void) {

	switch(26) {
	case 0:
		Sce_bgm_control(1, 0, 1, 40, 64);
	break;
	case 256:
		Sce_bgm_control(1, 0, 1, 60, 64);
	break;
	case 512:
		Sce_bgm_control(1, 0, 1, 90, 22);
	break;
	case 768:
		Sce_bgm_control(1, 0, 1, 100, 105);
	break;
	case 1024:
		Sce_bgm_control(1, 0, 1, 90, 105);
	break;
	case 1280:
		Sce_bgm_control(1, 0, 1, 60, 64);
	break;
	case 1536:
		Sce_bgm_control(1, 0, 1, 40, 64);
	break;
	case 1792:
		Sce_bgm_control(1, 0, 1, 90, 22);
	break;
	case 2048:
		Sce_bgm_control(1, 0, 1, 60, 64);
	break;
	case 2304:
		Sce_bgm_control(1, 0, 1, 90, 22);
	break;
	}
	return 0;
}