#ifndef MERCEDF1_H__
#define MERCEDF1_H__

#include "..\Shared\DLLBase.h"

class EDF00Mission : public DLLBase//public SPMission 
{
public:
	EDF00Mission(void)
	{
		m_GameTPS = 10; // default

		AllowRandomTracks(false); // Allow random music if the user wants it
		b_count = &b_last - &b_first - 1;
		b_array = &b_first + 1;

		f_count = &f_last - &f_first - 1;
		f_array = &f_first + 1;

		h_count = &h_last - &h_first - 1;
		h_array = &h_first + 1;

		i_count = &i_last - &i_first - 1;
		i_array = &i_first + 1;

		v_count = &v_last - &v_first - 1;
		v_array = &v_first + 1;
	}

	void Init(void);
	void AddObject(Handle h);
	bool Load(bool missionSave);
	bool PostLoad(bool missionSave);
	bool Save(bool missionSave);
	void Execute(void);

	// bools
	bool
		b_first,
		/*ON_HOLD,start_done,shab_down_path1,shab_down_path2,shab_down_path3,
		shab_at_point1,shab_at_point2,shab_at_point3,shab_at_cliff,shab_down_tight,
		shab_at_base,player_lost,player_lost2,set_lost_times,second_warning,game_over,
		squad1_start,squad1_off,shab_shot,first_time,roger_message,message1,base_center,
		left_base,shab_to_center,shab_centered,search_base,return_to_base, return_to_base2,
		encouragement,first_leave,wingman_away,shab_to_tunnel,comm_identified,johns_orders,
		comm_help,find_herself,shab_at_comm,ship_destroyed,wingman_message1,shab_response,
		shab_warning1,shab_response1,john_in_ship,on_route,base_detected,throttle_down,
		in_range,targeted, scion1_target, scion2_target,in_combat,battle1_over,roger1,
		wingman_report,next_wave1,returned,second_dead,base_return,
		
		mayday_message1, mayday_message2, mayday_message3, mayday_message4, mayday_message5,
		next_detected,john_away,attack2,freed,john_close,battle2_over,thanks_message,
		red1_return,coward2,truck_found, no_find_truck,truck_response1,truck_response2,
		truck_response3,truck_return,last_wave_dead,last_spawned,out_of_ship,inside, fire_message,
		at_player, squad1_astop, squad1_bstop, squad1_cstop, jumped, cheat_death, giveahand, bubba,
		cliff_fall, player_there,wingat_player, single_dead, bang_up_message, return_message, 
		radio_for_help, alt_message1,alt_message2, alt_message3, alt_message4, to_goalone, change_look1, 
		goto_sbay, last_message, last_attack, gus, the_end, tubby, wait_4_wing, wing_orders, go_shab_go,
		at_sbay, last_message2, squad1_talk, wing_look, wing_first, wingman_there, shab_through,
		to_comm, shab_wait, done, shab_hold, wing_hold, plane_time_set, reroute1, player_off,

		stayput,door_open,release_crew,up_quake,landed,pilot_speak1,pilot_speak2,pilot_speak3,
		landed_message,free_player,clear_warning,remove_condor1,remove_condor2,go_shab_go2,
		turret_detected,turret_retreat,shab_turret_comment,shab_cutoff_turret,turret_dead,
		turret_deploy,turret_hostle,alert_skyeye,simms_turret_comment,shab_weapon_comment,
		wing_lookat_turret,in_comm,cineractive_over,simms_tank_comment,first_scion_dead,
		simms_panic,player_awl,squad1_stopped,what_doing,didnt_listen,at_doors,
		crate1_dead,crate2_dead,crate3_dead,crate4_dead,crate5_dead,crate6_dead,crate7_dead,crate8_dead,
		last_crate_dead,red_look_truck,very_last_message,extra_spawned,hold_fire_message,
		start_cineractive,line2,line3,line4,line5,line6,line7,
		camera1_over,camera2_over,first_frame,fade_done,first_squad_removed,
		camera3_over,camera4_over,movie1,movie2,reframe1,chatter1,chatter_set,*/
		m_DidOneTimeInit,
		m_ConvoyContinueToBase,
		m_EnableFailCheck,
		m_runPowerAIStateMachine,
		m_runPowerPlayerStateMachine,
		m_Routine4Enable,
		m_CerbRoutine,
		m_PreventPowerDamage,
		m_SetGun10Heath,
		b_last;

	// floats
	float
		f_first,
		//wait_time,wait1_time,wait2_time,player_lost_time,lost_warning_time,
		//return_check_time,shot_me_time,message_time,cliff_check,base_check_time,
		//comm_check_time,shab_comm_check,ship_destroyed_time,john_ship_check,
		//dialog1_time,dialog2_time,on_route_check,coward_check,coward_message,
		//next_wave_time,return_time,away_check,attack_check,red1_check,truck_check,
		//away_check2,truck_lost_check,truck_talk_time,spawn_check,out_check, fire_reset,
		//look_time, squad1_time, john_check, whatever, wing_look_time, lula_time,
		//to_alone_check, facing_time, last_check, wingman_check, tunnel_check,planes_wait,
		//open_doors_time,quake_time,pilot_speak_time,release_player_time,
		//off_drop_check,condor2_remove_time,condor1_remove_time,tit_time,wingy,
		//really_last_time,emit_time,camera_time,fade_time,next_shot_time,chatter_time,
		//what_doing_wait,  // [N1] new vars
		f_last;

	// handles
	Handle
		h_first,
		/*squad1_a, squad1_b, squad1_c,
		squad2_a, squad2_b, squad2_c,
		player,
		wingman,
		shabayev,
		comm_building,
		player_ship,
		scion1, scion2, scion3,
		red1, red2, truck, talk,
		storage_bay,
		power_up1, power_up2, power_up3, power_up4,
		condor1, condor2,
		holder1,holder2,holder3,
		nav,speak,turret,
		dead_tank,magic_crate,
		crate1,crate2,crate3,crate4,crate5,crate6,crate7,crate8,
		last_crate,
		crumble_cliff,
		smoker1,smoker2,smoker3,smoker4,
		look_thing,manson,
		line,pilot,*/
		Object_Player,
		Object_CarrierLaunchCamDummy,
		Object_LandingZone,
		Object_ConvoyHalt,
		Object_HardingNav,
		Object_Red,
		Object_Blue,
		Object_Radar1,
		Object_Radar2,
		Object_Gun1,
		Object_Gun2,
		Object_Gun3,
		Object_Gun4,
		Object_Gun5,
		Object_Gun6,
		Object_Gun7,
		Object_Gun8,
		Object_Gun9,
		Object_Gun10,
		Object_ServiceBay,
		Object_Carrier,
		Object_WyndtEssex,
		Object_Hardin,
		Object_Scout1,
		Object_Scout2,
		Object_Scout3,
		Object_ServTruck1,
		Object_ServTruck2,
		Object_Cargo1,
		Object_Cargo2,
		Object_Condor,
		Object_Corbernav,
		Object_Stayput,
		Object_Power1,
		Object_Power2,
		Object_Power3,
		Object_Power4,
		Object_Power5,
		Object_Power6,
		Object_Power7,
		Object_Krudernav,
		Object_Rodnav1,
		Object_Rodnav2,
		Object_NavForCerbUnit,
		Object_Nadir1,
		Object_Nadir2,
		Object_Nadir3,
		Object_Nadir4,
		Object_CerbUnit,
		h_last;

	// integers
	int
		i_first,
		/*x, y, a,
		h1,a1,
		h2,a2,
		quake_level,*/
		m_ElapsedGameTime,
		m_convoyStateMachine,
		m_convoyWaitTillTime,
		m_powerAIStateMachine,
		m_powerPlayerStateMachine,
		m_powerPlayerWaitTillTime,
		m_Routine4StateMachine,
		m_Routine4WaitTillTime,
		m_CerbStateMachine,
		m_CerbWaitTillTime,
		i_last;

	Vector
		v_first,
		Position_LandingZone3,
		Position_LandingZone1,
		Position_LandingZone0,
		Position_HardingNav1,
		Position_HardingNav2,
		Position_ScoutSpawning2,
		Position_ScoutSpawning1,
		Position_ServiceBay,
		Position_LandingZone,
		Position_LandingZone2,
		Position_Rodnav2,
		Position_NavForCerbUnit,
		Position_CarrierLaunchCamDummy,
		v_last;

protected:
	bool *b_array;
	int b_count;

	float *f_array;
	int f_count;

	int *h_array;
	int h_count;

	int *i_array;
	int i_count;

	Vector *v_array;
	int v_count;

private:
	int m_GameTPS;
	char TempMsgString[1024];

	int m_convoyStateMachineLast;
	int m_powerAIStateMachineLast;
	int m_powerPlayerStateMachineLast;
	int m_Routine4StateMachineLast;
	int m_CerbStateMachineLast;
};

#endif // MERCEDF1_H__