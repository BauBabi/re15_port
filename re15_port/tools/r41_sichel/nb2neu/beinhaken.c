                {   /* Runde 41 SF NB6 Messhaken RE15_BEIN_LOG=<datei>: Beingelenke (Knochen 0-6: Becken, Huefte/Knie/Knoechel
                     * rechts und links) jedes gezeichneten Typs 0x30 in Weltkoordinaten nach allen Posen-Haken, je Bild.
                     * Nur Messung (Dossier SF_sichel.md "NB6 Beinhaltung"), im Normalpfad stumm, kein Verhalten. */
                    static FILE *s_bein_fp = NULL; static int s_bein_tried = 0;
                    if (!s_bein_tried) { s_bein_tried = 1; const char *bp = getenv("RE15_BEIN_LOG"); if (bp && *bp) s_bein_fp = fopen(bp, "w"); }
                    if (s_bein_fp && npc->type == 0x30u && npc_skel->bone_count >= 7) {
                        fprintf(s_bein_fp, "[bein] F%u slot%d mo=%d af=%d gier=%d", (unsigned)g_engine.frame_count,
                                (int)(npc - g_actors), (int)npc->motion, (int)npc->anim_frame, (int)npc->rot_y);
                        for (int bb = 0; bb < 7; bb++) {
                            int32_t bw[3];
                            re15_skel_bone_to_world(npc_poses[bb].trans, npc->rot_y, npc->x, npc->y, npc->z, bw);
                            fprintf(s_bein_fp, " %d(%d,%d,%d)", bb, (int)bw[0], (int)bw[1], (int)bw[2]);
                        }
                        fputc('\n', s_bein_fp); fflush(s_bein_fp);
                    }
                }
