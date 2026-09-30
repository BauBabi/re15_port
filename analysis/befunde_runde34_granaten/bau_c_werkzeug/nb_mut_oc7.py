# Mutationsprobe M1/O-C7: alte Reihenfolge (Parse NACH re15_room_apply_pending) wiederherstellen.
p1 = "re15_port/platform/pc/src/room_pc.c"
s = open(p1, encoding="utf-8").read()
alt = "        re15_pc_room_esp_laden();\n"
assert s.count(alt) == 1
s = s.replace(alt, "        /* MUTATION O-C7: Aufruf entfernt */\n")
open(p1, "w", encoding="utf-8", newline="").write(s)
p2 = "re15_port/platform/pc/main.c"
s = open(p2, encoding="utf-8").read()
marke = "                             * pc_load_room_esp(rdt_buf, rdt_size, dest_room), also DANACH. */\n"
assert s.count(marke) == 1
s = s.replace(marke, marke + "                            pc_load_room_esp(rdt_buf, rdt_size, dest_room);   /* MUTATION O-C7 */\n")
open(p2, "w", encoding="utf-8", newline="").write(s)
print("mutiert")
