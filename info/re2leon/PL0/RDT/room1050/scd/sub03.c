int sub03(void) {

	switch(26) {
	case 0:
	case 1280:
		Sce_bgm_control(0, 0, 1, 0, 64);
		Sce_bgm_control(0, 0, 2, 60, 65);
		Sce_bgm_control(0, 0, 3, 60, 65);
	break;
	case 256:
	case 512:
		Sce_bgm_control(0, 0, 1, 0, 40);
		Sce_bgm_control(0, 0, 2, 60, 65);
		Sce_bgm_control(0, 0, 3, 60, 65);
	break;
	case 768:
	case 1024:
		Sce_bgm_control(0, 0, 1, 0, 90);
		Sce_bgm_control(0, 0, 2, 60, 65);
		Sce_bgm_control(0, 0, 3, 60, 65);
	break;
	case 1536:
		Sce_bgm_control(0, 0, 1, 0, 65);
		Sce_bgm_control(0, 0, 2, 50, 65);
		Sce_bgm_control(0, 0, 3, 50, 65);
	break;
	}
	return 0;
}