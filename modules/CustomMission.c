#include "mpmissions/dayzOffline.chernarusplus/modules/npc_spawner/NPCManager.c"
#include "mpmissions/dayzOffline.chernarusplus/modules/player_classes/PlayerClassLoader.c"

class CustomMission: MissionServer {	
	ref NPCManager npc_manager;
	
	void CustomMission() {
	}

	void ~CustomMission() {
	}

	override void OnMissionStart() {
		npc_manager = new NPCManager();
		this.MissionScript.Call(npc_manager, "OnUpdate", NULL); //threaded
	}

	override PlayerBase OnClientNewEvent(PlayerIdentity identity, vector pos, ParamsReadContext ctx) {
		string characterName;
		ProcessLoginData(ctx, m_top, m_bottom, m_shoes, m_skin);
		
		if (m_top == -1 || m_bottom == -1 || m_shoes == -1 || m_skin == -1) {
			characterName = GetGame().CreateRandomPlayer();
		}
		else {
			characterName = GetGame().ListAvailableCharacters().Get(m_skin);
		}
		
		if (CreateCharacter(identity, pos, ctx, characterName)) {
			EquipCharacter();
			GetGame().RPCSingleParam(m_player, ERPCs.RPC_CHARACTER_EQUIPPED, NULL, true, m_player.GetIdentity());
		}
		
		return m_player;
	}
	
	override PlayerBase CreateCharacter(PlayerIdentity identity, vector pos, ParamsReadContext ctx, string characterName) {
		Entity playerEnt;
		playerEnt = GetGame().CreatePlayer(identity, characterName, pos, 0, "NONE");
		Class.CastTo(m_player, playerEnt);
		GetGame().SelectPlayer(identity, m_player);
		return m_player;
	}

	override void StartingEquipSetup(PlayerBase player, bool clothesChosen) {
		string id = m_player.GetIdentity().GetPlainId();
		new PlayerClassLoader(player); //Another Module
	}
}