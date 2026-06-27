int sub04(void) {

	Set(1, 27, 1);
	Set(2, 7, 1);
	Set(2, 2, 1);
	Aot_reset(4, 4, 49, 768, 0, 0xFF);
	Sce_Item_lost(86);
	Cut_chg(0x07);
	goto Sub15();
	Evt_next();
	nop();
	Sleep(7680);
	Set(5, 0, 1);
	Evt_exec(255, 0x180D);
	Set(29, 10, 1);
	// unknown opcode (8B) found at 0x00000034
	// unknown opcode (8A) found at 0x0000003A
	Evt_exec(255, 0x1809);
	Evt_exec(255, 0x1808);
	Sleep(1280);
	Sce_espr3d_on(0, 0x06, 0x05, 0, 0, 0xC009, 29376, 29431, 3301, 0, 4, 0, 0);
	Se_on(2, 13, 1, 0, 0, 29376, 29431, 3301);
	Sleep(2560);
	Sce_espr3d_on(0, 0x06, 0x05, 0, 0, 0xC009, 29376, 29431, 3301, 0, 4, 0, 0);
	Se_on(2, 13, 1, 0, 0, 29376, 29431, 3301);
	Sleep(1280);
	// unknown opcode (8A) found at 0x00000098
	Evt_exec(255, 0x1807);
	Sleep(5120);
	// unknown opcode (8A) found at 0x000000A6
	Evt_exec(255, 0x1806);
	Sleep(4608);
	goto Sub05();
	Sleep(7680);
	Cut_old();
	nop();
	goto Sub17();
	Evt_next();
	Set(1, 27, 0);
	Set(2, 2, 0);
	Set(2, 7, 0);
	nop();
	return 0;
}