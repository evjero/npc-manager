#include "mpmissions/dayzOffline.chernarusplus/modules/npc_spawner/npcs/NPC.c"
#include "mpmissions/dayzOffline.chernarusplus/modules/npc_spawner/npcs/NPCFisher.c"

class NPCManager {
    ref map<string, NPC> m_aliveNpcs = new map<string, NPC>;

    void NPCManager() {}

    void OnUpdate() {
        Sleep( 3000.0 ); // 3 seconds
        GetGame().ChatPlayer(CCDirect, "NPCs Remaining: "+m_aliveNpcs.Count());
        //Print("m_aliveNpcs: "+m_aliveNpcs);
        if(m_aliveNpcs.Count() < 1) {
            TestSpawn();
        }
        OnUpdate();
    }

    void TestSpawn() {
		NPC npc = NPC.Cast(new NPCFisher(this));
        if(npc.Spawn()){
            m_aliveNpcs.Set(npc.GetId(), npc);
        }
    }

    void RemoveNPC(NPC npc) {
        m_aliveNpcs.Remove(npc.GetId());
    }

    void RequestSucide(NPC npc) {
        Print("NPC, "+npc.GetId()+", is requesting suicide!");
        Sleep(Math.RandomIntInclusive(8000,10000));
        RemoveNPC(npc);
        npc.CommitSuicide();
    }

    void RequestExplosion(NPC npc) {
        Print("Explosion in 3...2...");
        Sleep(1000);
        npc.Explode();
    }
}