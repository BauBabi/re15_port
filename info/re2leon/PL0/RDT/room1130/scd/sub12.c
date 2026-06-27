int sub12(void) {

	Work_set(3, 1);
	nop();
	Pos_set(0, 29351, 0, -2893);
	Dir_set(0, 0, 19205, 0);
	Sca_id_set(1, 0x0000);
	Sleep(256);
	Work_set(3, 1);
	nop();
	Plc_dest(0, 1314, -12381, 27821);
	goto Sub29();
	Work_set(3, 1);
	Plc_motion(0, 15, 0);
	nop();
	Sleep(8448);
	Work_set(3, 1);
	nop();
	Plc_dest(0, 2082, -2652, 31919);
	goto Sub29();
	Plc_dest(0, 1314, 29852, 5312);
	Plc_rot(0, 24576);
	goto Sub29();
	Plc_dest(0, 1314, 5276, 10466);
	goto Sub29();
	Wsleep();
	Wsleeping();
	Cut_chg(0x05);
	Work_set(1, 0);
	nop();
	Pos_set(0, 3226, 0, 8351);
	Work_set(3, 1);
	nop();
	Pos_set(0, 5276, 0, -32019);
	Plc_dest(0, 1314, 5276, -11782);
	goto Sub29();
	goto Sub13();
	return 0;
}