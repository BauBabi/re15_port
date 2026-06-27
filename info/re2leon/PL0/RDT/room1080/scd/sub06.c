int sub06(void) {

	Set(5, 1, 0);
	Evt_exec(255, 0x1807);
	Aot_reset(3, 0, 0, 0, 0, 0x00);
	do {
		Sleep(256);
	} while(Ck(5, 1, 0));
	return 0;
}