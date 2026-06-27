int sub10(void) {

	Work_set(3, 1);
	nop();
	Pos_set(0, -28766, 0, 7092);
	Dir_set(0, 0, 6383, 0);
	Sca_id_set(1, 0x0000);
	Sleep(256);
	Work_set(3, 1);
	nop();
	Plc_dest(0, 1314, -601, 24758);
	goto Sub29();
	Work_set(3, 1);
	Plc_motion(0, 15, 0);
	nop();
	Sleep(8448);
	Work_set(3, 1);
	nop();
	Plc_dest(0, 2082, 1446, 27573);
	goto Sub29();
	Plc_dest(0, 1314, -2403, 10942);
	Plc_rot(0, 24576);
	goto Sub29();
	Plc_dest(0, 1314, 5276, -32019);
	goto Sub29();
	Wsleep();
	Wsleeping();
	Cut_chg(0x05);
	Plc_dest(0, 1314, 5276, -11782);
	goto Sub29();
	goto Sub13();
	return 0;
}