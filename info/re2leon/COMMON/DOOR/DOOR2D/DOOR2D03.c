int DOOR2D03(void) {

	Work_set(5, 0);
	nop();
	for(n = 0; n < 7680; n++) {
		Member_copy(16, 13);
		nop();
		Calc(16 + 2560)
		Member_set2(13, 16);
		nop();
		Member_copy(16, 12);
		nop();
		Calc(16 - 2560)
		Member_set2(12, 16);
		nop();
		Sleep(512);
		Member_copy(16, 13);
		nop();
		Calc(16 - 2560)
		Member_set2(13, 16);
		nop();
		Member_copy(16, 12);
		nop();
		Calc(16 + 2560)
		Member_set2(12, 16);
		nop();
		Sleep(512);
	}
	for(n = 0; n < 1536; n++) {
		Sleep(6656);
		Member_copy(16, 13);
		nop();
		Calc(16 + 1280)
		Member_set2(13, 16);
		nop();
		Member_copy(16, 12);
		nop();
		Calc(16 - 1280)
		Member_set2(12, 16);
		nop();
		Sleep(512);
		Member_copy(16, 13);
		nop();
		Calc(16 - 1280)
		Member_set2(13, 16);
		nop();
		Member_copy(16, 12);
		nop();
		Calc(16 + 1280)
		Member_set2(12, 16);
		nop();
		Sleep(512);
	}
	for(n = 0; n < 2560; n++) {
		Member_copy(16, 13);
		nop();
		Calc(16 + 2048)
		Member_set2(13, 16);
		nop();
		Member_copy(16, 12);
		nop();
		Calc(16 - 2048)
		Member_set2(12, 16);
		nop();
		Sleep(512);
		Member_copy(16, 13);
		nop();
		Calc(16 - 2048)
		Member_set2(13, 16);
		nop();
		Member_copy(16, 12);
		nop();
		Calc(16 + 2048)
		Member_set2(12, 16);
		nop();
		Sleep(512);
	}
	return 0;
}