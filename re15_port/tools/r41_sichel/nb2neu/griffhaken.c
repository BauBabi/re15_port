                {   /* Runde 41 SF NB6 Messhaken RE15_GRIFF_LOG=<datei>: Griffpunkte der Hangar-Szene je gezeichnetem Bild nach allen
                     * Posen-Haken - Typ 0x30 Knochen 15/13/8 (Sichelhand, Schulter, Brust), Typ 0x2B Knochen 0 und 20 (Griffknochen),
                     * Weltkoordinaten mit Figurmatrix und Massstab wie der Zeichner. Nur Messung, im Normalpfad stumm, kein Verhalten. */
                    static FILE *s_griff_fp = NULL; static int s_griff_tried = 0;
                    if (!s_griff_tried) { s_griff_tried = 1; const char *gp = getenv("RE15_GRIFF_LOG"); if (gp && *gp) s_griff_fp = fopen(gp, "w"); }
                    if (s_griff_fp && (npc->type == 0x30u || npc->type == 0x2Bu) && npc_skel->bone_count > 20) {
                        static const int k30[] = { 15, 13, 8 }, k2b[] = { 0, 20 };
                        const int *kn = npc->type == 0x30u ? k30 : k2b, nk = npc->type == 0x30u ? 3 : 2;
                        fprintf(s_griff_fp, "[griff] F%u typ%02X mo=%d af=%d", (unsigned)g_engine.frame_count, (unsigned)npc->type,
                                (int)npc->motion, (int)npc->anim_frame);
                        for (int q = 0; q < nk; q++) {
                            int32_t l[3] = { npc_poses[kn[q]].trans[0], npc_poses[kn[q]].trans[1], npc_poses[kn[q]].trans[2] }, gw[3];
                            GRIFF_WELT
                            fprintf(s_griff_fp, " %d(%d,%d,%d)", kn[q], (int)gw[0], (int)gw[1], (int)gw[2]);
                        }
                        fputc('\n', s_griff_fp); fflush(s_griff_fp);
                    }
                }
