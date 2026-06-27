int sub03(void) {

	Set(4, 73, 1);
	Aot_reset(2, 0, 0, 0, 0, 0x00);
	Se_on(2, 10, 0, 0, 0, -20295, -30495, 22686);
	// unknown opcode (8B) found at 0x00000020
	Evt_exec(255, 0x1804);
	Sce_shake_on(0, 16777216);
	Sleep(512);
	Sce_shake_on(0, 16777216);
	Sleep(512);
	Sce_shake_on(0, 33554432);
	Sleep(2048);
	Evt_exec(255, 0x1805);
	return 0;
}