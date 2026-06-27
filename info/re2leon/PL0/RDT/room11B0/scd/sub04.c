int sub04(void) {

	Light_kido_set(0, -20426);
	Sleep(512);
	Light_kido_set(0, -17356);
	Sleep(768);
	Light_kido_set(0, -28621);
	goto(0x0000ECFF);
	return 0;
}