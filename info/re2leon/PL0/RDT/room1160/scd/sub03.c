int sub03(void) {

	switch(26) {
	case 0:
		Sce_bgm_control(0, 0, 2, 100, 68);
	break;
	case 256:
		Sce_bgm_control(0, 0, 2, 105, 58);
	break;
	case 512:
		Sce_bgm_control(0, 0, 2, 100, 65);
	break;
	}
	do {
		Sleep(256);
	} while(Ck(1, 11, 0));
	goto(0x0000C2FF);
	return 0;
}