int sub03(void) {

	Work_set(2, 0);
	nop();
	Sleep(256);
	Member_copy(16, 7);
	nop();
	Calc(16 && 65519)
	Member_set2(7, 16);
	nop();
	Plc_dest(0, 2083, -2915, 5791);
	goto Sub09();
	Xa_on(0, 10752);
	Plc_dest(0, 1315, 22711, 156);
	goto Sub09();
	Member_copy(16, 7);
	nop();
	Calc(16 || 16)
	Member_set2(7, 16);
	nop();
	Member_copy(16, 7);
	nop();
	Calc(16 || 128)
	Member_set2(7, 16);
	nop();
	Member_copy(16, 0);
	nop();
	Calc(16 || 512)
	Member_set2(0, 16);
	nop();
	Member_copy(16, 0);
	nop();
	Calc(16 || 2048)
	Member_set2(0, 16);
	nop();
	Pos_set(0, -18240, 0, 22736);
	Member_set(39, -18240);
	Member_set(40, 22736);
	Wsleep();
	Wsleeping();
	return 0;
}