int sub08(void) {

	if(Ck(5, 3, 0)) {
		Set(2, 7, 1);
		Set(1, 27, 1);
		Set(5, 3, 1);
		Xa_on(0, 7424);
		Sleep(512);
		Message_on(0, 2304, 0x0000);
		Work_set(3, 0);
		Plc_motion(0, 18, 0);
		nop();
		Sleep(5120);
		Plc_neck(2, 0, 0, 1, 1, 5140);
		Sleep(5120);
		Plc_neck(2, 0, 0, 255, 1, 5140);
		Sleep(5120);
		Plc_neck(2, 0, 0, 1, 1, 10280);
		Sleep(2560);
		Plc_neck(2, 0, 0, 255, 1, 10280);
		Sleep(3840);
		Wsleep();
		Wsleeping();
		Set(2, 7, 0);
		Set(1, 27, 0);
		Plc_motion(0, 22, 16);
		Sleep(15360);
		Work_set(3, 0);
		Plc_motion(0, 22, 4);
		nop();
		Plc_neck(2, 0, 0, 0, 1, 8224);
		Set(5, 3, 0);
	}
	return 0;
}