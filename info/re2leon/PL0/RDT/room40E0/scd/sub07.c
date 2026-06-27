int sub07(void) {

	Set(4, 93, 1);
	Se_on(2, 11, 0, 4, 0, 0, 0, 0);
	Set(24, 5, 1);
	Set(24, 6, 1);
	Evt_exec(255, 0x180E);
	Sleep(7680);
	goto Sub08();
	Work_set(4, 0);
	nop();
	Pos_set(0, 131, 131, 131);
	Set(5, 0, 1);
	Cut_chg(0x09);
	Cut_auto(1);
	Cut_replace(1, 6);
	Cut_replace(2, 7);
	Cut_replace(11, 8);
	Cut_replace(12, 9);
	Cut_replace(13, 10);
	nop();
	Sca_id_set(19, 0x80FC);
	Set(24, 5, 0);
	Set(24, 6, 0);
	return 0;
}