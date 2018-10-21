
class NPC {
    private int retries = 0;
    private string m_Id;
    private ref NPCManager m_manager;
    Human man;
    //EmoteCB m_EmoteCallback;

    //Derived classes will populate these
    int gender = 0; //0 = either, 1 = male, 2 = female
    ref array<vector> possible_positions;
    vector _defaultPos = Vector(9193.7,10,1935.7);
    m_orientation = Vector(0,0,0);

    //what the npc wears
    ref TStringArray headwear; 
    ref TStringArray torsowear; 
    ref TStringArray gloves; 
    ref TStringArray legwear; 
    ref TStringArray footwear;
    ref TStringArray gear; //backpacks, armbands, etc
    //what goes in inventory
    ref TStringArray loot; 
    ref TStringArray weapons;
    ref TStringArray weapon_ammo; //magazine - loose ammo goes in 'loot'
    ref TStringArray weapon_attachments;
    bool weaponInHand = false;
    bool explode = false;

    //Animation configs    
    bool lieDown = false;
    bool willSuicide = false;
    
	//TODO: thread NPC life w/ NPCBase
    void NPC(NPCManager npcm, Vector orientation) {
        m_manager = npcm;
        m_Id = "NPC_"+GetGame().GetTickTime().ToString(); //unqiue hash
        m_orientation = orientation;
        Setup();
    }

    void ~NPC() {}
    void Setup() {} //derived class defines

    string GetId() {
        return m_Id;
    }

    bool Spawn() {
        bool create_local = false;
        bool init_ai = true;

        string classname = "";
        switch(gender) {
            case 1:
                classname = maleSurvivors.GetRandomElement();
                break;
            case 2:
                classname = femaleSurvivors.GetRandomElement();
                break;
            default:
                classname = allSurvivors.GetRandomElement();
                break;
        }

        vector pos;
        if(possible_positions.Count() > 0 ) {
            pos = possible_positions.GetRandomElement();
        }else{
            Print("[Error] "+GetId()+" is using _defaultPos!");
            pos = _defaultPos;
        }
        return CreateNPCObject(classname,pos,create_local,init_ai);
    }

    bool CreateNPCObject(string classname, vector pos, bool create_local, bool init_ai) {
        PlayerBase obj = PlayerBase.Cast(GetGame().CreateObject(classname, pos, create_local, init_ai));
        GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(SetOrientation, 2000, false, true);
        if(man){
            AddGear();
            AddLoot();

            if(lieDown) {
                man.StartCommand_Action(DayZPlayerConstants.CMD_GESTUREFB_LYINGDOWN,EmoteCB,DayZPlayerConstants.STANCEMASK_ERECT);
            }
            if(willSuicide) {
                PrepareSuicide();
            }
            if(explode)
            {
                m_manager.RequestExplosion(this);
            }
        } else {
            retries = retries + 1;
            if(retries < 3){
                return CreateNPCObject(classname,pos,create_local,init_ai);
            }else{
                return false;
            }
        }
        return true;
    }

    void AddGear() {
        EntityAI e;
        ItemBase ib;
        //Clothing/Gear second for inventory space
        if(headwear.Count() > 0) {
            e = man.GetInventory().CreateInInventory(headwear.GetRandomElement());
            SetItemRandomHealth(e);
        }
        if(torsowear.Count() > 0) {
            e = man.GetInventory().CreateInInventory(torsowear.GetRandomElement());
            SetItemRandomHealth(e);
        }
        if(gloves.Count() > 0) {
            e = man.GetInventory().CreateInInventory(gloves.GetRandomElement());
            SetItemRandomHealth(e);
        }
        if(legwear.Count() > 0) {
            e = man.GetInventory().CreateInInventory(legwear.GetRandomElement());
            SetItemRandomHealth(e);
        }
        if(footwear.Count() > 0) {
            e = man.GetInventory().CreateInInventory(footwear.GetRandomElement());
            SetItemRandomHealth(e);
        }
        if(gear.Count() > 0) {
            e = man.GetInventory().CreateInInventory(gear.GetRandomElement());
            SetItemRandomHealth(e);
        }

        if(weapons.Count() > 0) {
            string weapon_class = weapons.GetRandomElement();
            EntityAI ent_weapon = man.GetInventory().CreateInInventory(weapon_class);
            if(ent_weapon) {
                SetItemRandomHealth(ent_weapon);
                if(weaponInHand) {
                    man.LocalTakeEntityToHands(ent_weapon);
                }
            }
            if(weapon_ammo.Count() > 0) {
                Magazine mag = Magazine.Cast(man.GetInventory().CreateInInventory(weapon_ammo.Get(0)));
                //Weapon_Base can only be cast if it is a firearm
                if(ent_weapon) {
                    if(Weapon_Base.CastTo(ent_weapon,GetItemInHands())) {
                        Weapon weapon = Weapon.Cast(ent_weapon);
                        int muzzle = weapon.GetCurrentMuzzle();
                        if(weapon.CanAttachMagazine(muzzle, mag)) {
                            weapon.AttachMagazine(muzzle, mag);
                        }
                    }
                }
            }
        }
    }

    void AddLoot() {
        EntityAI e;
        ItemBase ib;

        //Loot after all the gear
        for(int i = 0; i < loot.Count(); i++) {
            e = man.GetInventory().CreateInInventory(loot.Get(i));
            SetItemRandomHealth(e);
        }

        AddCustomLoot();
    }

    void AddCustomLoot() { /*Derived class override*/ }

    //Copy of logic for ID_EMOTE_SUICIDE in EmoteManager.c
    void PrepareSuicide() {
        int suicideID = DayZPlayerConstants.CMD_SUICIDEFB_UNARMED;
        string suicideStr;
        ItemBase weapon = GetItemInHands();
        if (weapon) {
			if (weapon.ConfigIsExisting("suicideAnim")) {
                suicideStr = weapon.ConfigGetString("suicideAnim");
            }
            if (weapon.IsKindOf("Pistol_Base")) {
                suicideID = DayZPlayerConstants.CMD_SUICIDEFB_PISTOL;
            }
            else if (weapon.IsKindOf("Rifle_Base")) {
                suicideID = DayZPlayerConstants.CMD_SUICIDEFB_RIFLE;
            }
            else if (suicideStr == "onehanded") 	suicideID = DayZPlayerConstants.CMD_SUICIDEFB_1HD;
            else if (suicideStr == "fireaxe") 		suicideID = DayZPlayerConstants.CMD_SUICIDEFB_FIREAXE;
            else if (suicideStr == "pitchfork") 	suicideID = DayZPlayerConstants.CMD_SUICIDEFB_PITCHFORK;
            else if (suicideStr == "sword") 		suicideID = DayZPlayerConstants.CMD_SUICIDEFB_SWORD;
            else if (suicideStr == "spear") 		suicideID = DayZPlayerConstants.CMD_SUICIDEFB_SPEAR;
            else if (suicideStr == "woodaxe") 		suicideID = DayZPlayerConstants.CMD_SUICIDEFB_WOODAXE;
            else //item in hands is not eligible for suicide
                suicideID = -1;
        }

        if (suicideID > -1) {
            Print("Attempt to request suicide with "+suicideID);
            Print(EmoteCB);
            Print(man);
            man.StartCommand_Action(suicideID,EmoteCB,DayZPlayerConstants.STANCEMASK_ERECT | DayZPlayerConstants.STANCEMASK_CROUCH);
            //GetGame().GameScript.Call(m_manager, "RequestSucide", new Param1<NPC>(this));
            //GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(this.SuicideUpdate, 0, true);
            m_manager.RequestSucide(this);
            
        }
        else {
            Print("Cannot suicide with this item");
        }
    }

    void CommitSuicide() {
		Weapon_Base weapon;
		WeaponEventBase weapon_event = new WeaponEventTrigger;
		
		if (Weapon_Base.CastTo(weapon,GetItemInHands())) {
            Print("Firearm suicide");
            int muzzle = weapon.GetCurrentMuzzle();
			//TODO : check multiple muzzles for shotguns, eventually
			if (weapon.CanFire()) {
				weapon.ProcessWeaponEvent(weapon_event);
                //weapon.Fire(muzzle);
                WeaponInventory wepin = WeaponInventory.Cast(weapon.GetInventory());
                wepin.TryFireWeapon(weapon, muzzle);
                KillNPC();
			}
			else {
				weapon.ProcessWeaponEvent(weapon_event);
			}
		}
		else if (GetItemInHands() && GetItemInHands().ConfigIsExisting("suicideAnim")) {
            Print("Melee suicide");
            EntityAI mwep;
            EntityAI.CastTo(mwep,GetItemInHands());
            SetItemHealth(mwep, 10);
            string soundset = "FNX45_Shot_SoundSet";
            CreateSound(soundset);
            KillNPC();
		}
		else {
            Print("Unarmed Suicide - Now will less blood!");
			KillNPC();
		}
	}

    void KillNPC() {
        if (GetGame().IsServer()) {
            if(man) { man.SetHealth(0); }
		}
    }

    ItemBase GetItemInHands() {
		ItemBase item = ItemBase.Cast( man.GetHumanInventory().GetEntityInHands() );
		return item;
	}

    void SetOrientation() {
        man.SetOrientation(m_orientation);
    }

    void CreateSound(string soundset) {
        SoundParams params;
        SoundObject obj;
        SoundObjectBuilder builder;
        
        params = new SoundParams(soundset);
        builder = new SoundObjectBuilder(params);
        obj = builder.BuildSoundObject();
        obj.SetPosition(man.GetPosition()); //EntityAI/Human
        GetGame().GetSoundScene().Play3D(obj,builder);
        int rpc = 0; //WIP
        GetGame().RPCSingleParam(NULL, rpc, new Param1<soundset>, false, NULL);
    }

    ref TStringArray maleSurvivors = {
        "SurvivorM_Mirek","SurvivorM_Boris","SurvivorM_Cyril","SurvivorM_Denis",
        "SurvivorM_Elias","SurvivorM_Francis","SurvivorM_Guo","SurvivorM_Hassan",
        "SurvivorM_Indar","SurvivorM_Jose","SurvivorM_Kaito","SurvivorM_Lewis",
        "SurvivorM_Manua","SurvivorM_Niki","SurvivorM_Oliver","SurvivorM_Peter",
        "SurvivorM_Quinn","SurvivorM_Rolf","SurvivorM_Seth","SurvivorM_Taiki",
    };

    ref TStringArray femaleSurvivors = {
        "SurvivorF_Eda","SurvivorF_Frida","SurvivorF_Gabi","SurvivorF_Helga",
        "SurvivorF_Irena","SurvivorF_Judy","SurvivorF_Keiko","SurvivorF_Lina",
        "SurvivorF_Maria","SurvivorF_Naomi",
    };

    ref TStringArray allSurvivors = {
        "SurvivorF_Eda","SurvivorF_Frida","SurvivorF_Gabi","SurvivorF_Helga",
        "SurvivorF_Irena","SurvivorF_Judy","SurvivorF_Keiko","SurvivorF_Lina",
        "SurvivorF_Maria","SurvivorF_Naomi",
        "SurvivorM_Mirek","SurvivorM_Boris","SurvivorM_Cyril","SurvivorM_Denis",
        "SurvivorM_Elias","SurvivorM_Francis","SurvivorM_Guo","SurvivorM_Hassan",
        "SurvivorM_Indar","SurvivorM_Jose","SurvivorM_Kaito","SurvivorM_Lewis",
        "SurvivorM_Manua","SurvivorM_Niki","SurvivorM_Oliver","SurvivorM_Peter",
        "SurvivorM_Quinn","SurvivorM_Rolf","SurvivorM_Seth","SurvivorM_Taiki",
    };

    void SetItemRandomHealth(EntityAI itemEnt) {
		int rndHlt = Math.RandomInt(40,70); //Nothing's perfect
		itemEnt.SetHealth("","",rndHlt);
	}

    void SetItemHealth(EntityAI itemEnt, int health) {
		itemEnt.SetHealth("","",health);
	}

    void Explode() {
        ItemBase item = man.GetInventory().CreateInInventory("RDG2SmokeGrenade_Black");
		item.Explode();
        //For safety
        KillNPC();
    }
}