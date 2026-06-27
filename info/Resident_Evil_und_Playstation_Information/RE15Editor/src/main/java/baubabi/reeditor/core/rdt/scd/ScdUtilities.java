package baubabi.reeditor.core.rdt.scd;

import baubabi.reeditor.core.exception.ExceptionHandler;
import baubabi.reeditor.core.rdt._40_main_scd.MainScd40;
import baubabi.reeditor.core.rdt._42_sub_scd.SubScd42;
import baubabi.reeditor.core.rdt._44_extra_scd.ExtraScd44;
import baubabi.reeditor.core.rdt.scd._00_op00_nop.NoOperation_nop_00;
import baubabi.reeditor.core.rdt.scd._01_op01_evt_end.ExitEvtAndReturn_evtEnd_01;
import baubabi.reeditor.core.rdt.scd._02_op02_evt_next.EventWaitForPlayerInput_evtNext_02;
import baubabi.reeditor.core.rdt.scd._03_op03_evt_chain.EventReinitializeScriptExecutionData_evtChain_03;
import baubabi.reeditor.core.rdt.scd._04_op04_evt_exec.EventExecution_evtExec_04;
import baubabi.reeditor.core.rdt.scd._05_op05_evt_kill.EventKill_evtKill_05;
import baubabi.reeditor.core.rdt.scd._06_op06_ifel_ck.IfCase_ifelCk_06;
import baubabi.reeditor.core.rdt.scd._07_op07_else_ck.ElseCase_elseCk_07;
import baubabi.reeditor.core.rdt.scd._08_op08_endif.EndIfCase_endif_08;
import baubabi.reeditor.core.rdt.scd._09_op09_sleep.Sleep_sleep_09;
import baubabi.reeditor.core.rdt.scd._100_op64_sce_espr_on2.SceEsprOn2_sceEsprOn2_64;
import baubabi.reeditor.core.rdt.scd._101_op65_sce_espr_kill2.SceEsprKill2_sceEsprKill2_65;
import baubabi.reeditor.core.rdt.scd._102_op66_plc_stop.PlcStop_plcStop_66;
import baubabi.reeditor.core.rdt.scd._103_op67_aot_set_4p.AddWall_aotSet4p_67;
import baubabi.reeditor.core.rdt.scd._104_op68_door_aot_set_4p.AddDoor_doorAotSet4p_68;
import baubabi.reeditor.core.rdt.scd._105_op69_item_aot_set_4p.AddItem_itemAotSet4p_69;
import baubabi.reeditor.core.rdt.scd._106_op6A_light_pos_set.SetLightPositionComponent_lightPosSet_6A;
import baubabi.reeditor.core.rdt.scd._107_op6B_light_kido_set.ChangeBrightnessOfLight_lightKidoSet_6B;
import baubabi.reeditor.core.rdt.scd._108_op6C_rbj_reset.RbjReset_rbjReset_6C;
import baubabi.reeditor.core.rdt.scd._109_op6D_sce_scr_move.SetFirstRowForBackgroundImage_sceScrMove_6D;
import baubabi.reeditor.core.rdt.scd._10_op0A_sleeping.BlockEventExecutionDuringSleep_sleeping_0A;
import baubabi.reeditor.core.rdt.scd._110_op6E_parts_set.SetValueInMemoryLocation_partsSet_6E;
import baubabi.reeditor.core.rdt.scd._111_op6F_movie_on.PlayMovie_movieOn_6F;
import baubabi.reeditor.core.rdt.scd._112_op70_splc_ret.SplcRet_splcRet_70;
import baubabi.reeditor.core.rdt.scd._113_op71_splc_sce.SplcSce_splcSce_71;
import baubabi.reeditor.core.rdt.scd._114_op72_super_on.SuperOn_superOn_72;
import baubabi.reeditor.core.rdt.scd._115_op73_mirror_set.MirrorSet_mirrorSet_73;
import baubabi.reeditor.core.rdt.scd._116_op74_sce_fade_adjust.SceFadeAdjust_sceFadeAdjust_74;
import baubabi.reeditor.core.rdt.scd._117_op75_sce_espr3d_on2.SceEspr3dOn2_sceEspr3dOn2_75;
import baubabi.reeditor.core.rdt.scd._118_op76_sce_item_get.AddItemToInventory_sceItemGet_76;
import baubabi.reeditor.core.rdt.scd._119_op77_sce_line_start.SceLineStart_sceLineStart_77;
import baubabi.reeditor.core.rdt.scd._11_op0B_wsleep.InitializeGlobalSleep_wsleep_0B;
import baubabi.reeditor.core.rdt.scd._120_op78_sce_line_main.SceLineMain_sceLineMain_78;
import baubabi.reeditor.core.rdt.scd._121_op79_sce_line_end.SceLineEnd_sceLineEnd_79;
import baubabi.reeditor.core.rdt.scd._122_op7A_sce_parts_bomb.ScePartsBomb_scePartsBomb_7A;
import baubabi.reeditor.core.rdt.scd._123_op7B_sce_parts_down.ScePartsDown_scePartsDown_7B;
import baubabi.reeditor.core.rdt.scd._124_op7C_light_color_set.SetCamLightColor_lightColorSet_7C;
import baubabi.reeditor.core.rdt.scd._125_op7D_light_pos_set2.SetLightColorForCam_lightPosSet2_7D;
import baubabi.reeditor.core.rdt.scd._126_op7E_light_kido_set2.ChangeLightBrightnessForCam_lightKidoSet2_7E;
import baubabi.reeditor.core.rdt.scd._127_op7F_light_color_set2.SetLightColorForCam_lightColorSet2_7F;
import baubabi.reeditor.core.rdt.scd._128_op80_se_vol.MultiplyValues_seVol_80;
import baubabi.reeditor.core.rdt.scd._129_op81_perf.PerformComparison_perf_81;
import baubabi.reeditor.core.rdt.scd._12_op0C_wsleeping.ExecuteGlobalSleep_wsleeping_0C;
import baubabi.reeditor.core.rdt.scd._130_op82_unknown2.Unknown2_unknown2_82;
import baubabi.reeditor.core.rdt.scd._131_op83_unknown3.Unknown3_unknown3_83;
import baubabi.reeditor.core.rdt.scd._132_op84_unknown4.Unknown4_unknown4_84;
import baubabi.reeditor.core.rdt.scd._133_op85_unknown5.UnknownSomethingOffset6_unknown5_85;
import baubabi.reeditor.core.rdt.scd._134_op86_poison_ck.CheckPoisonStatus_poisonCk_86;
import baubabi.reeditor.core.rdt.scd._135_op87_poison_clr.ClearPoisonStatus_poisonClr_87;
import baubabi.reeditor.core.rdt.scd._136_op88_sce_item_ck_lost.CheckItemInInventoryAndRemove_sceItemCkLost_88;
import baubabi.reeditor.core.rdt.scd._137_op89_unknown6.Unknown6_unknown6_89;
import baubabi.reeditor.core.rdt.scd._138_op8A_nop8a.Nop8a_nop8a_8A;
import baubabi.reeditor.core.rdt.scd._139_op8B_nop8b.Nop8b_nop8b_8B;
import baubabi.reeditor.core.rdt.scd._13_op0D_for.ForLoop_for_0D;
import baubabi.reeditor.core.rdt.scd._140_op8C_nop8c.Nop8c_nop8c_8C;
import baubabi.reeditor.core.rdt.scd._141_op8D_unknown7.UnknownSomethingRoomObjList_unknown7_8D;
import baubabi.reeditor.core.rdt.scd._142_op8E_unknown8.UnknownSomethingWithEnemies_unknown8_8E;
import baubabi.reeditor.core.rdt.scd._143_unknown_some_weird_return.UnknownSomeWeirdReturn_weirdReturn_FF;
import baubabi.reeditor.core.rdt.scd._14_op0E_next.EndForLoop_next_0E;
import baubabi.reeditor.core.rdt.scd._15_op0F_while.WhileLoop_while_0F;
import baubabi.reeditor.core.rdt.scd._16_op10_ewhile.EndWhileLoop_ewhile_10;
import baubabi.reeditor.core.rdt.scd._17_op11_do.StartDoLoop_do_11;
import baubabi.reeditor.core.rdt.scd._18_op12_edwhile.EndDoLoop_edwhile_12;
import baubabi.reeditor.core.rdt.scd._19_op13_switch.StartSwitch_switch_13;
import baubabi.reeditor.core.rdt.scd._20_op14_case.StartCaseBlock_case_14;
import baubabi.reeditor.core.rdt.scd._21_op15_default.StartDefaultBlock_default_15;
import baubabi.reeditor.core.rdt.scd._22_op16_eswitch.EndSwitchBlock_eswitch_16;
import baubabi.reeditor.core.rdt.scd._23_op17_goto.GotoAdress_goto_17;
import baubabi.reeditor.core.rdt.scd._24_op18_gosub.GotoSub_gosub_18;
import baubabi.reeditor.core.rdt.scd._25_op19_return.ReturnGosub_return_19;
import baubabi.reeditor.core.rdt.scd._26_op1A_break.BreakExecutingBlock_break_1A;
import baubabi.reeditor.core.rdt.scd._27_op1B_for2.ForLoop2_for2_1B;
import baubabi.reeditor.core.rdt.scd._28_op1C_break_point.BreakPoint_breakPoint_1C;
import baubabi.reeditor.core.rdt.scd._29_op1D_work_copy.ChangeInstruction_workCopy_1D;
import baubabi.reeditor.core.rdt.scd._30_op1E_nop1e.Nop1e_nop1e_1E;
import baubabi.reeditor.core.rdt.scd._31_op1F_nop1f.Nop1f_nop1f_1F;
import baubabi.reeditor.core.rdt.scd._32_op20_nop20.Nop20_nop20_20;
import baubabi.reeditor.core.rdt.scd._33_op21_ck.TestBitInArray_ck_21;
import baubabi.reeditor.core.rdt.scd._34_op22_set.SetBitInArray_set_22;
import baubabi.reeditor.core.rdt.scd._35_op23_compare.CompareValueAgainstArrayValue_compare_23;
import baubabi.reeditor.core.rdt.scd._36_op24_save.SetVariableValue_save_24;
import baubabi.reeditor.core.rdt.scd._37_op25_copy.CopyWordFromSourceToDestination_copy_25;
import baubabi.reeditor.core.rdt.scd._38_op26_calc.OperateOnValueUsingValue_calc_26;
import baubabi.reeditor.core.rdt.scd._39_op27_calc2.OperateOnVariableUsingVariable_calc2_27;
import baubabi.reeditor.core.rdt.scd._40_op28_sce_rnd.SceneRender_sceRnd_28;
import baubabi.reeditor.core.rdt.scd._41_op29_cut_chg.SetActiveCamera_cutChg_29;
import baubabi.reeditor.core.rdt.scd._42_op2A_cut_old.OldCam_cutOld_2A;
import baubabi.reeditor.core.rdt.scd._43_op2B_message_on.MessageOn_messageOn_2B;
import baubabi.reeditor.core.rdt.scd._44_op2C_aot_set.DefineNonPickableObject_aotSet_2C;
import baubabi.reeditor.core.rdt.scd._45_op2D_obj_model_set.ObjectModel_objModelSet_2D;
import baubabi.reeditor.core.rdt.scd._46_op2E_work_set.SelectObjectForInstruction_workSet_2E;
import baubabi.reeditor.core.rdt.scd._47_op2F_speed_set.SetInternalRegister_speedSet_2F;
import baubabi.reeditor.core.rdt.scd._48_op30_add_speed.SetInternalRegister_addSpeed_30;
import baubabi.reeditor.core.rdt.scd._49_op31_add_aspeed.AddSixWords_addAspeed_31;
import baubabi.reeditor.core.rdt.scd._50_op32_pos_set.SetPosition_posSet_32;
import baubabi.reeditor.core.rdt.scd._51_op33_dir_set.SetDirection_dirSet_33;
import baubabi.reeditor.core.rdt.scd._52_op34_member_set.MemberSet_memberSet_34;
import baubabi.reeditor.core.rdt.scd._53_op35_member_set2.MemberSet2_memberSet2_35;
import baubabi.reeditor.core.rdt.scd._54_op36_se_on.SeOn_seOn_36;
import baubabi.reeditor.core.rdt.scd._55_op37_sca_id_set.ChangeCamera_scaIdSet_37;
import baubabi.reeditor.core.rdt.scd._56_op38_flr_set.FloorSet_flrSet_38;
import baubabi.reeditor.core.rdt.scd._57_op39_dir_ck.DirectoryCk_dirCk_39;
import baubabi.reeditor.core.rdt.scd._58_op3A_sce_espr_on.AnimationAboveBackground_sceEsprOn_3A;
import baubabi.reeditor.core.rdt.scd._59_op3B_door_aot_set.DoorObject_doorAotSet_3B;
import baubabi.reeditor.core.rdt.scd._60_op3C_cut_auto.ShowScreen_cutAuto_3C;
import baubabi.reeditor.core.rdt.scd._61_op3D_member_copy.CopyEntityValueToVariable_memberCopy_3D;
import baubabi.reeditor.core.rdt.scd._62_op3E_member_cmp.CompareValueAgainstMemoryLocation_memberCmp_3E;
import baubabi.reeditor.core.rdt.scd._63_op3F_plc_motion.PlcAnimation_plcMotion_3F;
import baubabi.reeditor.core.rdt.scd._64_op40_plc_dest.EntityAnimationAndDirection_plcDest_40;
import baubabi.reeditor.core.rdt.scd._65_op41_plc_neck.PlcNeck_plcNeck_41;
import baubabi.reeditor.core.rdt.scd._66_op42_plc_ret.ChangeToStatusScreen_plcRet_42;
import baubabi.reeditor.core.rdt.scd._67_op43_plc_flg.PlcFlg_plcFlg_43;
import baubabi.reeditor.core.rdt.scd._68_op44_sce_em_set.Enemy_sceEmSet_44;
import baubabi.reeditor.core.rdt.scd._69_op45_col_chg_set.ColumnChangeSet_colChgSet_45;
import baubabi.reeditor.core.rdt.scd._70_op46_aot_reset.ActionPlayerTriggers_aotReset_46;
import baubabi.reeditor.core.rdt.scd._71_op47_aot_on.ActivateObject_aotOn_47;
import baubabi.reeditor.core.rdt.scd._72_op48_super_set.SuperSet_superSet_48;
import baubabi.reeditor.core.rdt.scd._73_op49_super_reset.SuperReset_superReset_49;
import baubabi.reeditor.core.rdt.scd._74_op4A_plc_gun.SetPlayerGun_plcGun_4A;
import baubabi.reeditor.core.rdt.scd._75_op4B_cut_replace.SwapValueOfCameras_cutReplace_4B;
import baubabi.reeditor.core.rdt.scd._76_op4C_sce_espr_kill.SceEsprKill_sceEsprKill_4C;
import baubabi.reeditor.core.rdt.scd._77_op4D_unknown1.Unknown1_unknown1_4D;
import baubabi.reeditor.core.rdt.scd._78_op4E_item_aot_set.AddItemToRoom_itemAotSet_4E;
import baubabi.reeditor.core.rdt.scd._79_op4F_sce_key_ck.SceKeyCk_sceKeyCk_4F;
import baubabi.reeditor.core.rdt.scd._80_op50_another_item_aot_set.AddItemToRoom_AnotherItemAotSet_50;
import baubabi.reeditor.core.rdt.scd._81_op51_sce_key_ck_renamed.SceKeyCk_sceKeyCk_51;
import baubabi.reeditor.core.rdt.scd._82_op52_sce_espr_control.SceEsprControl_sceEsprControl_52;
import baubabi.reeditor.core.rdt.scd._83_op53_sce_fade_set.FadeSettings_sceFadeSet_53;
import baubabi.reeditor.core.rdt.scd._84_op54_sce_espr_control.SceEspControl_sceEsprControl_54;
import baubabi.reeditor.core.rdt.scd._85_op55_member_calc.MemberCalc_memberCalc_55;
import baubabi.reeditor.core.rdt.scd._86_op56_member_calc2.MemberCalc2_memberCalc2_56;
import baubabi.reeditor.core.rdt.scd._87_op57_sce_bgmtbl_set.SceBgmtblSet_sceBgmtblSet_57;
import baubabi.reeditor.core.rdt.scd._88_op58_plc_rot.PlayerRotation_PlcRot_58;
import baubabi.reeditor.core.rdt.scd._89_op59_xa_on.PlaySound_xaOn_59;
import baubabi.reeditor.core.rdt.scd._90_op5A_weapon_chg.CheckPlayerInventory_weaponChg_5A;
import baubabi.reeditor.core.rdt.scd._91_op5B_plc_cnt.PlcCnt_plcCnt_5B;
import baubabi.reeditor.core.rdt.scd._92_op5C_sce_shake_on.SceShakeOn_sceShakeOn_5C;
import baubabi.reeditor.core.rdt.scd._93_op5D_mizu_div_set.ModelCreation_mizuDivSet_5D;
import baubabi.reeditor.core.rdt.scd._94_op5E_keep_item_ck.CheckPlayerItem_keepItemCk_5E;
import baubabi.reeditor.core.rdt.scd._95_op5F_xa_vol.VolumeSetting_xaVol_5F;
import baubabi.reeditor.core.rdt.scd._96_op60_kage_set.KageSet_kageSet_60;
import baubabi.reeditor.core.rdt.scd._97_op61_cut_be_set.CameraSwitchSetting_cutBeSet_61;
import baubabi.reeditor.core.rdt.scd._98_op62_sce_item_lost.RemoveItemAfterUsage_sceItemLost_62;
import baubabi.reeditor.core.rdt.scd._99_op63_plc_gun_eff.GunEffect_plcGunEff_63;
import lombok.Getter;
import lombok.extern.log4j.Log4j2;

import java.lang.reflect.InvocationTargetException;
import java.lang.reflect.Method;
import java.util.*;

@Getter
@Log4j2
public class ScdUtilities {
    private static ScdUtilities INSTANCE;

    Map<String, Integer> opcodeByteMap = new HashMap<>();

    LinkedHashMap<String, String> enemyMapForSelection = new LinkedHashMap<>();

    LinkedHashMap<String, String> enemyMapForSaving = new LinkedHashMap<>();

    LinkedHashMap<String, String> stateMapForSelection = new LinkedHashMap<>();

    LinkedHashMap<String, String> stateMapForSaving = new LinkedHashMap<>();

    List<Class> scdElementList = new ArrayList<>();


    private ScdUtilities() {
        prepareClassList();

        for(int i = 0; i < scdElementList.size(); i++) {
            Class scdElementClass = scdElementList.get(i);
            Method opcodeMethod = null;
            Method sizeMethod = null;
            try {
                opcodeMethod = scdElementClass.getDeclaredMethod("getOPCODE");
            } catch (NoSuchMethodException e) {
                ExceptionHandler.getInstance().logException("NoSuchMethodException at creating method for getting Opcode", e);
            }
            try {
                sizeMethod = scdElementClass.getDeclaredMethod("getSize");
            } catch (NoSuchMethodException e) {
                ExceptionHandler.getInstance().logException("NoSuchMethodException at creating method for getting Scd Element size", e);
            }
            Object scdClassInstance = null;
            try {
                scdClassInstance = scdElementClass.getDeclaredConstructor().newInstance();
            } catch (InstantiationException e) {
                ExceptionHandler.getInstance().logException("InstantiationException at creating create class instance for scd utilities", e);
            } catch (IllegalAccessException e) {
                ExceptionHandler.getInstance().logException("IllegalAccessException at creating create class instance for scd utilities", e);
            } catch (InvocationTargetException e) {
                ExceptionHandler.getInstance().logException("InvocationTargetException at creating create class instance for scd utilities", e);
            } catch (NoSuchMethodException e) {
                ExceptionHandler.getInstance().logException("NoSuchMethodException at creating create class instance for scd utilities", e);
            }

            try {
                opcodeByteMap.put((String) opcodeMethod.invoke(scdClassInstance), (Integer) sizeMethod.invoke(scdClassInstance));
            } catch (IllegalAccessException e) {
                ExceptionHandler.getInstance().logException("IllegalAccessException at adding scd opcode and scd size to opcodeByteMap", e);
            } catch (InvocationTargetException e) {
                ExceptionHandler.getInstance().logException("InvocationTargetException at adding scd opcode and scd size to opcodeByteMap", e);
            }
        }

        enemyMapForSelection.put("10", "10 - POLICE ZOMBIE SLIM - Stage 1,2,3,4,5");
        enemyMapForSelection.put("11", "11 - POLICE ZOMBIE FAT GREY HAIR - Stage 1,2,3,4,5");
        enemyMapForSelection.put("12", "12 - CROCODILE - Stage 1,2,3,4,5");
        enemyMapForSelection.put("13", "13 - CIVIL ZOMBIE FEMALE - Stage 1,2,3,4,5");
        enemyMapForSelection.put("16", "16 - CIVIL ZOMBIE - Stage 1,2,3,4,5");
        enemyMapForSelection.put("18", "18 - LAB ZOMBIE - Stage 1,2,3,4,5");
        enemyMapForSelection.put("1A", "1A - ZOMBIE UPPER BODY - Stage 1,2");
        enemyMapForSelection.put("20", "20 - DOG - Stage 1,3,5");
        enemyMapForSelection.put("21", "21 - CROW - Stage 1,4,5");
        enemyMapForSelection.put("24", "24 - WATER (FLOODING) - Stage 2");
        enemyMapForSelection.put("25", "25 - SPIDER - Stage 2,3,6");
        enemyMapForSelection.put("26", "26 - UNKNOWN - Stage 1");
        enemyMapForSelection.put("27", "27 - MONKEY - Stage 1");
        enemyMapForSelection.put("29", "29 - MONKEY - Stage 4,5");
        enemyMapForSelection.put("2A", "2A - SOMETHING");
        enemyMapForSelection.put("2B", "2B - SPIDER-MAN - Stage 4,5");
        enemyMapForSelection.put("2D", "2D - FUSE DEVICE - Stage 4");
        enemyMapForSelection.put("30", "30 - BIRKIN - Stage 3,5");
        enemyMapForSelection.put("36", "36 - BIRKIN UPPER BODY - Stage 3");
        enemyMapForSelection.put("40", "40 - MARVIN - Stage 1,2,3,4,5,6");
        enemyMapForSelection.put("42", "42 - ADA - Stage 1,2,3,4,5,6");
        enemyMapForSelection.put("45", "45 - IRONS - Stage 1,6");
        enemyMapForSelection.put("47", "47 - IRONS (Leon), ROY (Elza) - Stage 1,5");
        enemyMapForSelection.put("49", "49 - KENDO - Stage 1,2,3,4,5,6");
        enemyMapForSelection.put("4B", "4B - SHERRY - Stage 1,2,3,4,5,6");
        enemyMapForSelection.put("4D", "4D - ANNETTE - Stage 5,6");
        enemyMapForSelection.put("58", "58 - POLICE ZOMBIE SLIM WITH STROKE - (Leon) - Stage 3");
        enemyMapForSelection.put("5B", "5B - FEMALE ZOMBIE STANDING (Leon) - Stage 1,2,3,4,5,6");

        for (Map.Entry<String, String> entry : enemyMapForSelection.entrySet()) {
            enemyMapForSaving.put(entry.getValue(), entry.getKey());
        }

        stateMapForSelection.put("0", "000 - regular behavior - enemy attack when nearing, Partner follow");
        stateMapForSelection.put("2", "002 - stand until nearing (ZOMBIE)");
        stateMapForSelection.put("6", "006 - eating, stand up when nearing (ZOMBIE)");
        stateMapForSelection.put("13", "013 - slow walk at player (ZOMBIE)");
        stateMapForSelection.put("14", "014 - sits and stand up when nearing (ZOMBIE)");
        stateMapForSelection.put("16", "016 - more on ground (CROW)");
        stateMapForSelection.put("36", "036 - only stands (BIRKIN UPPER BODY)");
        stateMapForSelection.put("48", "048 - sits, does not attack (MONKEY)");
        stateMapForSelection.put("51", "051 - stands, does not move (BIRKIN)");
        stateMapForSelection.put("64", "064 - stands without moving (PARTNER)");
        stateMapForSelection.put("65", "065 - lay sideway, does not attack (DOG), attacks (SPIDER), not there (SPIDER-MAN)");
        stateMapForSelection.put("97", "097 - sits, does not attack (MONKEY)");
        stateMapForSelection.put("129", "129 - lay on belly, crawl at player when nearing (ZOMBIE)");
        stateMapForSelection.put("131", "131 - lay on belly, does not move, but attacks when nearing (ZOMBIE)");
        stateMapForSelection.put("132", "132 - lay on belly, stands up when nearing (ZOMBIE)");
        stateMapForSelection.put("133", "133 - lay on back, stands up when nearing (ZOMBIE)");
        stateMapForSelection.put("134", "134 - eating, stands up when nearing (ZOMBIE)");
        stateMapForSelection.put("135", "135 - lay on belly, does not stand up or attack (ZOMBIE)");
        stateMapForSelection.put("136", "136 - lay on back, does not stand up or attack (ZOMBIE)");
        stateMapForSelection.put("144", "144 - more lively to attack (CROW)");
        stateMapForSelection.put("161", "161 - lay on belly, does not stand up or attack (ZOMBIE)");
        stateMapForSelection.put("174", "174 - sits, does not stand up or attack (ZOMBIE)");

        for (Map.Entry<String, String> entry : stateMapForSelection.entrySet()) {
            stateMapForSaving.put(entry.getValue(), entry.getKey());
        }
    }

    private void prepareClassList() {
        scdElementList.add(NoOperation_nop_00.class);
        scdElementList.add(ExitEvtAndReturn_evtEnd_01.class);
        scdElementList.add(EventWaitForPlayerInput_evtNext_02.class);
        scdElementList.add(EventReinitializeScriptExecutionData_evtChain_03.class);
        scdElementList.add(EventExecution_evtExec_04.class);
        scdElementList.add(EventKill_evtKill_05.class);
        scdElementList.add(IfCase_ifelCk_06.class);
        scdElementList.add(ElseCase_elseCk_07.class);
        scdElementList.add(EndIfCase_endif_08.class);
        scdElementList.add(Sleep_sleep_09.class);
        scdElementList.add(BlockEventExecutionDuringSleep_sleeping_0A.class);
        scdElementList.add(InitializeGlobalSleep_wsleep_0B.class);
        scdElementList.add(ExecuteGlobalSleep_wsleeping_0C.class);
        scdElementList.add(ForLoop_for_0D.class);
        scdElementList.add(EndForLoop_next_0E.class);
        scdElementList.add(WhileLoop_while_0F.class);
        scdElementList.add(EndWhileLoop_ewhile_10.class);
        scdElementList.add(StartDoLoop_do_11.class);
        scdElementList.add(EndDoLoop_edwhile_12.class);
        scdElementList.add(StartSwitch_switch_13.class);
        scdElementList.add(StartCaseBlock_case_14.class);
        scdElementList.add(StartDefaultBlock_default_15.class);
        scdElementList.add(EndSwitchBlock_eswitch_16.class);
        scdElementList.add(GotoAdress_goto_17.class);
        scdElementList.add(GotoSub_gosub_18.class);
        scdElementList.add(ReturnGosub_return_19.class);
        scdElementList.add(BreakExecutingBlock_break_1A.class);
        scdElementList.add(ForLoop2_for2_1B.class);
        scdElementList.add(BreakPoint_breakPoint_1C.class);
        scdElementList.add(ChangeInstruction_workCopy_1D.class);
        scdElementList.add(Nop1e_nop1e_1E.class);
        scdElementList.add(Nop1f_nop1f_1F.class);
        scdElementList.add(Nop20_nop20_20.class);
        scdElementList.add(TestBitInArray_ck_21.class);
        scdElementList.add(SetBitInArray_set_22.class);
        scdElementList.add(CompareValueAgainstArrayValue_compare_23.class);
        scdElementList.add(SetVariableValue_save_24.class);
        scdElementList.add(CopyWordFromSourceToDestination_copy_25.class);
        scdElementList.add(OperateOnValueUsingValue_calc_26.class);
        scdElementList.add(OperateOnVariableUsingVariable_calc2_27.class);
        scdElementList.add(SceneRender_sceRnd_28.class);
        scdElementList.add(SetActiveCamera_cutChg_29.class);
        scdElementList.add(OldCam_cutOld_2A.class);
        scdElementList.add(MessageOn_messageOn_2B.class);
        scdElementList.add(DefineNonPickableObject_aotSet_2C.class);
        scdElementList.add(ObjectModel_objModelSet_2D.class);
        scdElementList.add(SelectObjectForInstruction_workSet_2E.class);
        scdElementList.add(SetInternalRegister_speedSet_2F.class);
        scdElementList.add(SetInternalRegister_addSpeed_30.class);
        scdElementList.add(AddSixWords_addAspeed_31.class);
        scdElementList.add(SetPosition_posSet_32.class);
        scdElementList.add(SetDirection_dirSet_33.class);
        scdElementList.add(MemberSet_memberSet_34.class);
        scdElementList.add(MemberSet2_memberSet2_35.class);
        scdElementList.add(SeOn_seOn_36.class);
        scdElementList.add(ChangeCamera_scaIdSet_37.class);
        scdElementList.add(FloorSet_flrSet_38.class);
        scdElementList.add(DirectoryCk_dirCk_39.class);
        scdElementList.add(AnimationAboveBackground_sceEsprOn_3A.class);
        scdElementList.add(DoorObject_doorAotSet_3B.class);
        scdElementList.add(ShowScreen_cutAuto_3C.class);
        scdElementList.add(CopyEntityValueToVariable_memberCopy_3D.class);
        scdElementList.add(CompareValueAgainstMemoryLocation_memberCmp_3E.class);
        scdElementList.add(PlcAnimation_plcMotion_3F.class);
        scdElementList.add(EntityAnimationAndDirection_plcDest_40.class);
        scdElementList.add(PlcNeck_plcNeck_41.class);
        scdElementList.add(ChangeToStatusScreen_plcRet_42.class);
        scdElementList.add(PlcFlg_plcFlg_43.class);
        scdElementList.add(Enemy_sceEmSet_44.class);
        scdElementList.add(ColumnChangeSet_colChgSet_45.class);
        scdElementList.add(ActionPlayerTriggers_aotReset_46.class);
        scdElementList.add(ActivateObject_aotOn_47.class);
        scdElementList.add(SuperSet_superSet_48.class);
        scdElementList.add(SuperReset_superReset_49.class);
        scdElementList.add(SetPlayerGun_plcGun_4A.class);
        scdElementList.add(SwapValueOfCameras_cutReplace_4B.class);
        scdElementList.add(SceEsprKill_sceEsprKill_4C.class);
        scdElementList.add(Unknown1_unknown1_4D.class);
        scdElementList.add(AddItemToRoom_itemAotSet_4E.class);
        scdElementList.add(SceKeyCk_sceKeyCk_4F.class);
        scdElementList.add(AddItemToRoom_AnotherItemAotSet_50.class);
        scdElementList.add(SceKeyCk_sceKeyCk_51.class);
        scdElementList.add(SceEsprControl_sceEsprControl_52.class);
        scdElementList.add(FadeSettings_sceFadeSet_53.class);
        scdElementList.add(SceEspControl_sceEsprControl_54.class);
        scdElementList.add(MemberCalc_memberCalc_55.class);
        scdElementList.add(MemberCalc2_memberCalc2_56.class);
        scdElementList.add(SceBgmtblSet_sceBgmtblSet_57.class);
        scdElementList.add(PlayerRotation_PlcRot_58.class);
        scdElementList.add(PlaySound_xaOn_59.class);
        scdElementList.add(CheckPlayerInventory_weaponChg_5A.class);
        scdElementList.add(PlcCnt_plcCnt_5B.class);
        scdElementList.add(SceShakeOn_sceShakeOn_5C.class);
        scdElementList.add(ModelCreation_mizuDivSet_5D.class);
        scdElementList.add(CheckPlayerItem_keepItemCk_5E.class);
        scdElementList.add(VolumeSetting_xaVol_5F.class);
        scdElementList.add(KageSet_kageSet_60.class);
        scdElementList.add(CameraSwitchSetting_cutBeSet_61.class);
        scdElementList.add(RemoveItemAfterUsage_sceItemLost_62.class);
        scdElementList.add(GunEffect_plcGunEff_63.class);
        scdElementList.add(SceEsprOn2_sceEsprOn2_64.class);
        scdElementList.add(SceEsprKill2_sceEsprKill2_65.class);
        scdElementList.add(PlcStop_plcStop_66.class);
        scdElementList.add(AddWall_aotSet4p_67.class);
        scdElementList.add(AddDoor_doorAotSet4p_68.class);
        scdElementList.add(AddItem_itemAotSet4p_69.class);
        scdElementList.add(SetLightPositionComponent_lightPosSet_6A.class);
        scdElementList.add(ChangeBrightnessOfLight_lightKidoSet_6B.class);
        scdElementList.add(RbjReset_rbjReset_6C.class);
        scdElementList.add(SetFirstRowForBackgroundImage_sceScrMove_6D.class);
        scdElementList.add(SetValueInMemoryLocation_partsSet_6E.class);
        scdElementList.add(PlayMovie_movieOn_6F.class);
        scdElementList.add(SplcRet_splcRet_70.class);
        scdElementList.add(SplcSce_splcSce_71.class);
        scdElementList.add(SuperOn_superOn_72.class);
        scdElementList.add(MirrorSet_mirrorSet_73.class);
        scdElementList.add(SceFadeAdjust_sceFadeAdjust_74.class);
        scdElementList.add(SceEspr3dOn2_sceEspr3dOn2_75.class);
        scdElementList.add(AddItemToInventory_sceItemGet_76.class);
        scdElementList.add(SceLineStart_sceLineStart_77.class);
        scdElementList.add(SceLineMain_sceLineMain_78.class);
        scdElementList.add(SceLineEnd_sceLineEnd_79.class);
        scdElementList.add(ScePartsBomb_scePartsBomb_7A.class);
        scdElementList.add(ScePartsDown_scePartsDown_7B.class);
        scdElementList.add(SetCamLightColor_lightColorSet_7C.class);
        scdElementList.add(SetLightColorForCam_lightPosSet2_7D.class);
        scdElementList.add(ChangeLightBrightnessForCam_lightKidoSet2_7E.class);
        scdElementList.add(SetLightColorForCam_lightColorSet2_7F.class);
        scdElementList.add(MultiplyValues_seVol_80.class);
        scdElementList.add(PerformComparison_perf_81.class);
        scdElementList.add(Unknown2_unknown2_82.class);
        scdElementList.add(Unknown3_unknown3_83.class);
        scdElementList.add(Unknown4_unknown4_84.class);
        scdElementList.add(UnknownSomethingOffset6_unknown5_85.class);
        scdElementList.add(CheckPoisonStatus_poisonCk_86.class);
        scdElementList.add(ClearPoisonStatus_poisonClr_87.class);
        scdElementList.add(CheckItemInInventoryAndRemove_sceItemCkLost_88.class);
        scdElementList.add(Unknown6_unknown6_89.class);
        scdElementList.add(Nop8a_nop8a_8A.class);
        scdElementList.add(Nop8b_nop8b_8B.class);
        scdElementList.add(Nop8c_nop8c_8C.class);
        scdElementList.add(UnknownSomethingRoomObjList_unknown7_8D.class);
        scdElementList.add(UnknownSomethingWithEnemies_unknown8_8E.class);
        scdElementList.add(UnknownSomeWeirdReturn_weirdReturn_FF.class);
    }

    public static ScdUtilities getInstance() {
        if(INSTANCE == null) {
            INSTANCE = new ScdUtilities();
        }

        return INSTANCE;
    }

    public void setSingleScdDataElements(Object scdDataElement) {
        List<Object> opcodeDataElements = new ArrayList<>();

        String scdData = "";
        MainScd40 mainScd40 = null;
        SubScd42 subScd42 = null;
        ExtraScd44 extraScd44 = null;

        System.out.println();
        if(scdDataElement instanceof MainScd40) {
            mainScd40 = (MainScd40) scdDataElement;
            scdData = mainScd40.getScdData();
            System.out.println("SCD KIND - MAIN:");
            System.out.println();
        }
        else if(scdDataElement instanceof SubScd42) {
            subScd42 = (SubScd42) scdDataElement;
            scdData = subScd42.getScdData();
            System.out.println("SCD KIND - SUB:");
            System.out.println();
        }
        else if(scdDataElement instanceof ExtraScd44) {
            extraScd44 = (ExtraScd44) scdDataElement;
            scdData = extraScd44.getScdData();
            System.out.println("SCD KIND - EXTRA:");
            System.out.println();
        }

        int opcodeDataStart;
        int opcodeDataEnd = 0;

        for(int j = 0; j < scdData.length(); j++) {
            String opcode = scdData.substring(opcodeDataEnd,opcodeDataEnd + 2);
            int opcodeByteDataLength = fetchByteLengthOfOpcodeByteMap(opcode);

            //Its the aot_set data, and we have to check, if the 4th byte start with B1, if so it is 22 byte instead of 20.
            if(opcode.equals("2C")) {
                //Get the data and see what the result would be
                String byteForDataNext = scdData.substring(opcodeDataEnd,opcodeDataEnd + opcodeByteDataLength * 2);
                if(byteForDataNext.startsWith("B1", 6)) {
                    opcodeByteDataLength = opcodeByteDataLength + 2;
                }
            }

            opcodeDataStart = opcodeDataEnd;
            opcodeDataEnd = opcodeDataEnd + opcodeByteDataLength * 2;

            String byteForDataNext = scdData.substring(opcodeDataStart,opcodeDataEnd);
            opcodeDataElements.add(getScdClassForOpcode(opcode, byteForDataNext));

            j = opcodeDataEnd - 1;
            System.out.println("Byte of Opcode: " + byteForDataNext);
        }

        if(scdDataElement instanceof MainScd40) {
            mainScd40.setSingleScdDataElements(opcodeDataElements);
            System.out.println();
        }
        else if(scdDataElement instanceof SubScd42) {
            subScd42.setSingleScdDataElements(opcodeDataElements);
            System.out.println();
        }
        else if(scdDataElement instanceof ExtraScd44) {
            extraScd44.setSingleScdDataElements(opcodeDataElements);
            System.out.println();
        }
    }

    private Integer fetchByteLengthOfOpcodeByteMap(String opcode) {
        return opcodeByteMap.get(opcode);
    }

    private Object getScdClassForOpcode(String opcode, String param) {
        switch (opcode) {
            case "00":
                return new NoOperation_nop_00();
            case "01":
                return new ExitEvtAndReturn_evtEnd_01(param);
            case "02":
                return new EventWaitForPlayerInput_evtNext_02();
            case "03":
                return new EventReinitializeScriptExecutionData_evtChain_03(param);
            case "04":
                return new EventExecution_evtExec_04(param);
            case "05":
                return new EventKill_evtKill_05(param);
            case "06":
                return new IfCase_ifelCk_06(param);
            case "07":
                return new ElseCase_elseCk_07(param);
            case "08":
                return new EndIfCase_endif_08();
            case "09":
                return new Sleep_sleep_09(param);
            case "0A":
                return new BlockEventExecutionDuringSleep_sleeping_0A(param);
            case "0B":
                return new InitializeGlobalSleep_wsleep_0B();
            case "0C":
                return new ExecuteGlobalSleep_wsleeping_0C();
            case "0D":
                return new ForLoop_for_0D(param);
            case "0E":
                return new EndForLoop_next_0E(param);
            case "0F":
                return new WhileLoop_while_0F(param);
            case "10":
                return new EndWhileLoop_ewhile_10(param);
            case "11":
                return new StartDoLoop_do_11(param);
            case "12":
                return new EndDoLoop_edwhile_12(param);
            case "13":
                return new StartSwitch_switch_13(param);
            case "14":
                return new StartCaseBlock_case_14(param);
            case "15":
                return new StartDefaultBlock_default_15(param);
            case "16":
                return new EndSwitchBlock_eswitch_16(param);
            case "17":
                return new GotoAdress_goto_17(param);
            case "18":
                return new GotoSub_gosub_18(param);
            case "19":
                return new ReturnGosub_return_19(param);
            case "1A":
                return new BreakExecutingBlock_break_1A(param);
            case "1B":
                return new ForLoop2_for2_1B(param);
            case "1C":
                return new BreakPoint_breakPoint_1C();
            case "1D":
                return new ChangeInstruction_workCopy_1D(param);
            case "1E":
                return new Nop1e_nop1e_1E();
            case "1F":
                return new Nop1f_nop1f_1F();
            case "20":
                return new Nop20_nop20_20();
            case "21":
                return new TestBitInArray_ck_21(param);
            case "22":
                return new SetBitInArray_set_22(param);
            case "23":
                return new CompareValueAgainstArrayValue_compare_23(param);
            case "24":
                return new SetVariableValue_save_24(param);
            case "25":
                return new CopyWordFromSourceToDestination_copy_25(param);
            case "26":
                return new OperateOnValueUsingValue_calc_26(param);
            case "27":
                return new OperateOnVariableUsingVariable_calc2_27(param);
            case "28":
                return new SceneRender_sceRnd_28();
            case "29":
                return new SetActiveCamera_cutChg_29(param);
            case "2A":
                return new OldCam_cutOld_2A();
            case "2B":
                return new MessageOn_messageOn_2B(param);
            case "2C":
                if(param.length() == 40) {
                    return new DefineNonPickableObject_aotSet_2C(param);
                }
                else {
                    return new DefineNonPickableObject_aotSet_2C(param, true);
                }
            case "2D":
                return new ObjectModel_objModelSet_2D(param);
            case "2E":
                return new SelectObjectForInstruction_workSet_2E(param);
            case "2F":
                return new SetInternalRegister_speedSet_2F(param);
            case "30":
                return new SetInternalRegister_addSpeed_30();
            case "31":
                return new AddSixWords_addAspeed_31();
            case "32":
                return new SetPosition_posSet_32(param);
            case "33":
                return new SetDirection_dirSet_33(param);
            case "34":
                return new MemberSet_memberSet_34(param);
            case "35":
                return new MemberSet2_memberSet2_35(param);
            case "36":
                return new SeOn_seOn_36(param);
            case "37":
                return new ChangeCamera_scaIdSet_37(param);
            case "38":
                return new FloorSet_flrSet_38(param);
            case "39":
                return new DirectoryCk_dirCk_39(param);
            case "3A":
                return new AnimationAboveBackground_sceEsprOn_3A(param);
            case "3B":
                return new DoorObject_doorAotSet_3B(param);
            case "3C":
                return new ShowScreen_cutAuto_3C(param);
            case "3D":
                return new CopyEntityValueToVariable_memberCopy_3D(param);
            case "3E":
                return new CompareValueAgainstMemoryLocation_memberCmp_3E(param);
            case "3F":
                return new PlcAnimation_plcMotion_3F(param);
            case "40":
                return new EntityAnimationAndDirection_plcDest_40(param);
            case "41":
                return new PlcNeck_plcNeck_41(param);
            case "42":
                return new ChangeToStatusScreen_plcRet_42();
            case "43":
                return new PlcFlg_plcFlg_43(param);
            case "44":
                return new Enemy_sceEmSet_44(param);
            case "45":
                return new ColumnChangeSet_colChgSet_45(param);
            case "46":
                return new ActionPlayerTriggers_aotReset_46(param);
            case "47":
                return new ActivateObject_aotOn_47(param);
            case "48":
                return new SuperSet_superSet_48(param);
            case "49":
                return new SuperReset_superReset_49(param);
            case "4A":
                return new SetPlayerGun_plcGun_4A(param);
            case "4B":
                return new SwapValueOfCameras_cutReplace_4B(param);
            case "4C":
                return new SceEsprKill_sceEsprKill_4C(param);
            case "4D":
                return new Unknown1_unknown1_4D(param);
            case "4E":
                return new AddItemToRoom_itemAotSet_4E(param);
            case "4F":
                return new SceKeyCk_sceKeyCk_4F(param);
            case "50":
                return new AddItemToRoom_AnotherItemAotSet_50(param);
            case "51":
                return new SceKeyCk_sceKeyCk_51(param);
            case "52":
                return new SceEsprControl_sceEsprControl_52(param);
            case "53":
                return new FadeSettings_sceFadeSet_53(param);
            case "54":
                return new SceEspControl_sceEsprControl_54(param);
            case "55":
                return new MemberCalc_memberCalc_55(param);
            case "56":
                return new MemberCalc2_memberCalc2_56(param);
            case "57":
                return new SceBgmtblSet_sceBgmtblSet_57(param);
            case "58":
                return new PlayerRotation_PlcRot_58(param);
            case "59":
                return new PlaySound_xaOn_59(param);
            case "5A":
                return new CheckPlayerInventory_weaponChg_5A(param);
            case "5B":
                return new PlcCnt_plcCnt_5B(param);
            case "5C":
                return new SceShakeOn_sceShakeOn_5C(param);
            case "5D":
                return new ModelCreation_mizuDivSet_5D(param);
            case "5E":
                return new CheckPlayerItem_keepItemCk_5E(param);
            case "5F":
                return new VolumeSetting_xaVol_5F(param);
            case "60":
                return new KageSet_kageSet_60(param);
            case "61":
                return new CameraSwitchSetting_cutBeSet_61(param);
            case "62":
                return new RemoveItemAfterUsage_sceItemLost_62(param);
            case "63":
                return new GunEffect_plcGunEff_63();
            case "64":
                return new SceEsprOn2_sceEsprOn2_64(param);
            case "65":
                return new SceEsprKill2_sceEsprKill2_65(param);
            case "66":
                return new PlcStop_plcStop_66();
            case "67":
                return new AddWall_aotSet4p_67(param);
            case "68":
                return new AddDoor_doorAotSet4p_68(param);
            case "69":
                return new AddItem_itemAotSet4p_69(param);
            case "6A":
                return new SetLightPositionComponent_lightPosSet_6A(param);
            case "6B":
                return new ChangeBrightnessOfLight_lightKidoSet_6B(param);
            case "6C":
                return new RbjReset_rbjReset_6C();
            case "6D":
                return new SetFirstRowForBackgroundImage_sceScrMove_6D(param);
            case "6E":
                return new SetValueInMemoryLocation_partsSet_6E(param);
            case "6F":
                return new PlayMovie_movieOn_6F(param);
            case "70":
                return new SplcRet_splcRet_70();
            case "71":
                return new SplcSce_splcSce_71();
            case "72":
                return new SuperOn_superOn_72(param);
            case "73":
                return new MirrorSet_mirrorSet_73(param);
            case "74":
                return new SceFadeAdjust_sceFadeAdjust_74(param);
            case "75":
                return new SceEspr3dOn2_sceEspr3dOn2_75(param);
            case "76":
                return new AddItemToInventory_sceItemGet_76(param);
            case "77":
                return new SceLineStart_sceLineStart_77(param);
            case "78":
                return new SceLineMain_sceLineMain_78(param);
            case "79":
                return new SceLineEnd_sceLineEnd_79();
            case "7A":
                return new ScePartsBomb_scePartsBomb_7A(param);
            case "7B":
                return new ScePartsDown_scePartsDown_7B(param);
            case "7C":
                return new SetCamLightColor_lightColorSet_7C(param);
            case "7D":
                return new SetLightColorForCam_lightPosSet2_7D(param);
            case "7E":
                return new ChangeLightBrightnessForCam_lightKidoSet2_7E(param);
            case "7F":
                return new SetLightColorForCam_lightColorSet2_7F(param);
            case "80":
                return new MultiplyValues_seVol_80(param);
            case "81":
                return new PerformComparison_perf_81(param);
            case "82":
                return new Unknown2_unknown2_82(param);
            case "83":
                return new Unknown3_unknown3_83();
            case "84":
                return new Unknown4_unknown4_84(param);
            case "85":
                return new UnknownSomethingOffset6_unknown5_85(param);
            case "86":
                return new CheckPoisonStatus_poisonCk_86();
            case "87":
                return new ClearPoisonStatus_poisonClr_87();
            case "88":
                return new CheckItemInInventoryAndRemove_sceItemCkLost_88(param);
            case "89":
                return new Unknown6_unknown6_89();
            case "8A":
                return new Nop8a_nop8a_8A(param);
            case "8B":
                return new Nop8b_nop8b_8B(param);
            case "8C":
                return new Nop8c_nop8c_8C(param);
            case "8D":
                return new UnknownSomethingRoomObjList_unknown7_8D(param);
            case "8E":
                return new UnknownSomethingWithEnemies_unknown8_8E(param);
            case "FF":
                return new UnknownSomeWeirdReturn_weirdReturn_FF(param);
            default:
                return null;
        }
    }
}
