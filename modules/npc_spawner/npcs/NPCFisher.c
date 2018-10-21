class NPCFisher extends NPC {
    override void Setup() {
        //GENERAL
        gender = 0;//NPCSpawnableGender.MALE;
        possible_positions = {
            Vector(9191.7,10,1936.7),
            Vector(9192.7,10,1933.7),
            Vector(9193.7,10,1936.7),
            Vector(9194.7,10,1933.7),
            Vector(9195.7,10,1936.7),
            Vector(9196.7,10,1933.7),
            Vector(9197.7,10,1936.7),
            Vector(9198.7,10,1933.7),
        }; //Vector(9193.7,-1,1935.7),

        //GEAR CONFIG
        headwear = { "BoonieHat_Blue", "BoonieHat_Olive" };
        torsowear = { "Raincoat_Blue", "Raincoat_Yellow", "Raincoat_Green" };
        gloves = {};
        legwear = { "Breeches_Blue", "Breeches_Blackcheck", "Breeches_Browncheck" };
        footwear = { "Wellies_Grey" };
        gear = { "DryBag_Blue" };
        loot = {
            "TunaCan","TunaCan","TunaCan","TunaCan",
            "TunaCan","TunaCan","TunaCan","TunaCan",
            "TunaCan","TunaCan","TunaCan","TunaCan",
        };

        weapons = { "HuntingKnife" };
        weapon_ammo = {};
        weapon_attachments = {};

        //ANIMATIONS CONFIG
        //lieDown = true;
        //willSuicide = true;
        //weaponInHand = true;
        //explode = true;
    }

    override void AddCustomLoot() {
        //man.GetInventory().CreateInInventory("Hook");
    }
}