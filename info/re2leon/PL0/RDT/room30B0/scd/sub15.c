int sub15(void) {

	Se_on(2, 15, 1, 4, 0, 0, 0, 0);
	Sleep(12800);
	Work_set(1, 0);
	Plc_motion(0, 23, 1);
	nop();
	Sleep(6400);
	Work_set(4, 2);
	nop();
	Super_set(0, 64, 142, 8192, -9728, 4608, -16133, 0, -16136);
	Sleep(1280);
	Se_on(2, 16, 1, 4, 0, 0, 0, 0);
	Sleep(5120);
	Work_set(1, 0);
	Plc_motion(0, 36, 1);
	nop();
	Sleep(7680);
	Xa_on(0, 17920);
	Message_on(0, 3072, 0x0000);
	Sleep(1280);
	Wsleep();
	Wsleeping();
	Work_set(1, 0);
	Plc_motion(0, 35, 1);
	nop();
	Sleep(5120);
	Xa_on(0, 18176);
	Message_on(0, 3328, 0x0000);
	Wsleep();
	Wsleeping();
	Sleep(2560);
	Set(4, 89, 1);
	Set(32, 1, 1);
	Work_set(4, 2);
	nop();
	Super_reset(0, 192, 251, 0, 0, 192, 248);
	Pos_set(0, 0, -8015, 0);
	Work_set(1, 0);
	Plc_ret();
	Set(2, 7, 0);
	Set(1, 27, 0);
	Cut_auto(1);
	// unknown opcode (84) found at 0x000000A8
	return 0;
}