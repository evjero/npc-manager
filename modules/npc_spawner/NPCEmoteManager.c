class NPCEmoteCB: HumanCommandActionCallback
{
    bool m_IsFullbody;
	NPCBase m_NPC;
	
	bool CancelCondition()
	{
		return false;
	}
	
	bool IsEmoteCallback()
	{
		return true;
	}
	
	override void OnAnimationEvent(int pEventID)
	{
		if (pEventID == 1)
		{
			m_NPC.GetEmoteManager().KillPlayer();
		}
	}
}

class NPCEmoteManager: EmoteManager
{
    NPCBase m_NPC;
    ItemBase m_item;
    NPCEmoteCB m_Callback;
    bool					m_bEmoteIsPlaying;
	protected bool			m_BelayedEmote;
	protected bool			m_ItemToHands;
	protected bool			m_ItemIsOn;
	protected bool			m_MouseButtonPressed;
	protected bool 			m_PlayerDies;
	protected bool 			m_controllsLocked;
	protected int  			m_GestureID;
	protected int			m_BelayedEmoteID;
	protected int			m_PreviousGestureID;
	protected int			m_CurrentGestureID;
	protected int 			m_LastMask;
	protected int 			m_RPSOutcome;

    void EmoteManager( NPCBase npc ) 
	{
		m_NPC = npc;
		m_ItemIsOn = false;
		m_RPSOutcome = -1;
	}

    void SetGesture(int id)
	{
		m_GestureID = id;
	}
	
	int GetGesture()
	{
		return m_GestureID;
	}

    //Called from NPC each frame
	void Update( float deltaT )
	{
		if ( m_Callback ) 
		{
			if(m_Callback.m_IsFullbody) 
			{
				if (m_CurrentGestureID == ID_EMOTE_SUICIDE)
				{
					if (m_Callback.GetState() == m_Callback.STATE_LOOP_LOOP)
					{
						CommitSuicide();
					}
					else
					{
						return;
					}
				}
				else
				{
					EndCallback();
				}
			}
			if ( m_LastMask != -1 && m_NPC.IsPlayerInStance(DayZPlayerConstants.STANCEMASK_PRONE) )
			{
				m_Callback.Cancel();
			}
			if ( m_MenuEmote && m_bEmoteIsPlaying )
			{
				EndCallback();
			}
		}
		else
		{
			if (m_bEmoteIsPlaying)
			{
				m_bEmoteIsPlaying = false;
				OnEmoteEnd();
			}
		}
	}
	
	void OnEmoteEnd()
	{
		if ( m_ItemToHands )
		{
			ShowItemInHands();
		}
		
		//if suicide has been completed, kill npc
		if ( m_PlayerDies )
		{
			m_NPC.SetHealth(0.0);
			return;
		}
		
		if ( m_BelayedEmote )
		{
			if ( m_PreviousGestureID != m_BelayedEmoteID )
			{
				PickEmote(m_BelayedEmoteID);
				//PlayEmote(m_BelayedEmoteID);
			}
			m_BelayedEmoteID = -1;
			m_BelayedEmote = false;
		}
		else
		{
			if ( m_NPC.GetActionManager() ) m_NPC.GetActionManager().EnableActions();
			m_NPC.m_InventorySoftLocked = false;
			
			if ( m_controllsLocked )
			{
				m_controllsLocked = false;
				m_NPC.GetInputController().OverrideAimChangeX(false,0);
				m_NPC.GetInputController().OverrideMovementSpeed(false,0);
			}
		}

		//! back to the default - shoot from camera - if not set already
		if (!m_NPC.IsShootingFromCamera()) m_NPC.OverrideShootFromCamera(true);
	}

    //Configure Emote parameters for callback here
	bool PlayEmote( int id )
	{
		if (!CanPlayEmote())
		{
			return false;
		}
		
		m_CurrentGestureID = id;
		if( id > 0)
		{
			switch ( id )
			{
				case ID_EMOTE_GREETING :
					if (!m_NPC.IsPlayerInStance(DayZPlayerConstants.STANCEMASK_PRONE))
					{
						CreateEmoteCallback(EmoteCB,DayZPlayerConstants.CMD_GESTUREMOD_GREETING,DayZPlayerConstants.STANCEMASK_CROUCH | DayZPlayerConstants.STANCEMASK_ERECT,false);
					}
					else
					{
						CreateEmoteCallback(EmoteCB,DayZPlayerConstants.CMD_GESTUREFB_GREETING,DayZPlayerConstants.STANCEMASK_PRONE,true);
					}
				break;
			
				case ID_EMOTE_SOS :
					if (!m_NPC.IsPlayerInStance(DayZPlayerConstants.STANCEMASK_PRONE)) 	CreateEmoteCallback(EmoteCB,DayZPlayerConstants.CMD_GESTUREFB_SOS,DayZPlayerConstants.STANCEMASK_ERECT,true);
					HideItemInHands();
				break;
			
				case ID_EMOTE_HEART :
					if (!m_NPC.IsPlayerInStance(DayZPlayerConstants.STANCEMASK_PRONE))
					{
						CreateEmoteCallback(EmoteCB,DayZPlayerConstants.CMD_GESTUREMOD_HEART,DayZPlayerConstants.STANCEMASK_CROUCH | DayZPlayerConstants.STANCEMASK_ERECT,false);
					}
					else
					{
						CreateEmoteCallback(EmoteCB,DayZPlayerConstants.CMD_GESTUREFB_HEART,DayZPlayerConstants.STANCEMASK_PRONE,true);
					}
					HideItemInHands();
				break;
			
				case ID_EMOTE_TAUNT :
					if (!m_NPC.IsPlayerInStance(DayZPlayerConstants.STANCEMASK_PRONE))
					{
						CreateEmoteCallback(EmoteCB,DayZPlayerConstants.CMD_GESTUREMOD_TAUNT,DayZPlayerConstants.STANCEMASK_CROUCH | DayZPlayerConstants.STANCEMASK_ERECT,false);
					}
					else
					{
						CreateEmoteCallback(EmoteCB,DayZPlayerConstants.CMD_GESTUREFB_TAUNT,DayZPlayerConstants.STANCEMASK_PRONE,true);
					}
				break;
			
				case ID_EMOTE_LYINGDOWN :
					CreateEmoteCallback(EmoteCB,DayZPlayerConstants.CMD_GESTUREFB_LYINGDOWN,DayZPlayerConstants.STANCEMASK_CROUCH,true);
					HideItemInHands();
				break;
			
				case ID_EMOTE_TAUNTKISS :
					if (!m_NPC.IsPlayerInStance(DayZPlayerConstants.STANCEMASK_PRONE))
					{
						CreateEmoteCallback(EmoteCB,DayZPlayerConstants.CMD_GESTUREMOD_TAUNTKISS,DayZPlayerConstants.STANCEMASK_CROUCH | DayZPlayerConstants.STANCEMASK_ERECT,false);
					}
					else
					{
						CreateEmoteCallback(EmoteCB,DayZPlayerConstants.CMD_GESTUREFB_TAUNTKISS,DayZPlayerConstants.STANCEMASK_PRONE,true);
					}
				break;
			
				case ID_EMOTE_POINT :
					if (!m_NPC.IsPlayerInStance(DayZPlayerConstants.STANCEMASK_PRONE))
					{
						CreateEmoteCallback(EmoteCB,DayZPlayerConstants.CMD_GESTUREMOD_POINT,DayZPlayerConstants.STANCEMASK_CROUCH | DayZPlayerConstants.STANCEMASK_ERECT,false);
					}
					else
					{
						CreateEmoteCallback(EmoteCB,DayZPlayerConstants.CMD_GESTUREFB_POINT,DayZPlayerConstants.STANCEMASK_PRONE,true);
					}
					//m_bEmoteIsPlaying = false;
				break;
				
				case ID_EMOTE_TAUNTELBOW :
					CreateEmoteCallback(EmoteCB,DayZPlayerConstants.CMD_GESTUREMOD_TAUNTELBOW,DayZPlayerConstants.STANCEMASK_CROUCH | DayZPlayerConstants.STANCEMASK_ERECT,false);
				break;
				
				case ID_EMOTE_THUMB :
					if (!m_NPC.IsPlayerInStance(DayZPlayerConstants.STANCEMASK_PRONE))
					{
						CreateEmoteCallback(EmoteCB,DayZPlayerConstants.CMD_GESTUREMOD_THUMB,DayZPlayerConstants.STANCEMASK_CROUCH | DayZPlayerConstants.STANCEMASK_ERECT,false);
					}
					else
					{
						CreateEmoteCallback(EmoteCB,DayZPlayerConstants.CMD_GESTUREFB_THUMB,DayZPlayerConstants.STANCEMASK_PRONE,true);
					}
				break;
				
				case ID_EMOTE_THUMBDOWN :
					if (!m_NPC.IsPlayerInStance(DayZPlayerConstants.STANCEMASK_PRONE))
					{
						CreateEmoteCallback(EmoteCB,DayZPlayerConstants.CMD_GESTUREMOD_THUMBDOWN,DayZPlayerConstants.STANCEMASK_CROUCH | DayZPlayerConstants.STANCEMASK_ERECT,false);
					}
					else
					{
						CreateEmoteCallback(EmoteCB,DayZPlayerConstants.CMD_GESTUREFB_THUMBDOWN,DayZPlayerConstants.STANCEMASK_PRONE,true);
					}
				break;
				
				case ID_EMOTE_THROAT :
					if (!m_NPC.IsPlayerInStance(DayZPlayerConstants.STANCEMASK_PRONE))
					{
						CreateEmoteCallback(EmoteCB,DayZPlayerConstants.CMD_GESTUREMOD_THROAT,DayZPlayerConstants.STANCEMASK_CROUCH | DayZPlayerConstants.STANCEMASK_ERECT,false);
					}
					else
					{
						CreateEmoteCallback(EmoteCB,DayZPlayerConstants.CMD_GESTUREFB_THROAT,DayZPlayerConstants.STANCEMASK_PRONE,true);
					}
				break;
			
				case ID_EMOTE_SUICIDE :
					int suicideID = DayZPlayerConstants.CMD_SUICIDEFB_UNARMED;
					string suicideStr;
					ItemBase weapon;
					weapon = m_NPC.GetItemInHands();
					//suicideID = DayZPlayerConstants.CMD_SUICIDEFB_UNARMED; //unarmed suicide...optional?
					if (weapon)
					{
						if (weapon.ConfigIsExisting("suicideAnim"))
						{
							suicideStr = weapon.ConfigGetString("suicideAnim");
						}
						
						if (weapon.IsKindOf("Pistol_Base"))
						{
							suicideID = DayZPlayerConstants.CMD_SUICIDEFB_PISTOL;
						}

						else if (weapon.IsKindOf("Rifle_Base"))
						{
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
			
					if (suicideID > -1)
					{
						CreateEmoteCallback(EmoteCB,suicideID,DayZPlayerConstants.STANCEMASK_ERECT | DayZPlayerConstants.STANCEMASK_CROUCH,true);
						//m_Callback.RegisterAnimationEvent("Death",1);
					}
				break;
			
				case ID_EMOTE_DANCE:
					if (!m_NPC.IsPlayerInStance(DayZPlayerConstants.STANCEMASK_PRONE)) 	CreateEmoteCallback(EmoteCB,DayZPlayerConstants.CMD_GESTUREFB_DANCE,DayZPlayerConstants.STANCEMASK_ERECT,true);
					HideItemInHands();
				break;
				
				case ID_EMOTE_SALUTE:
					if (!m_NPC.IsPlayerInStance(DayZPlayerConstants.STANCEMASK_PRONE)) 	CreateEmoteCallback(EmoteCB,DayZPlayerConstants.CMD_GESTUREFB_SALUTE,DayZPlayerConstants.STANCEMASK_ERECT,true);
					//HideItemInHands();
				break;
				
				case ID_EMOTE_TIMEOUT:
					if (!m_NPC.IsPlayerInStance(DayZPlayerConstants.STANCEMASK_PRONE))
					{
						CreateEmoteCallback(EmoteCB,DayZPlayerConstants.CMD_GESTUREMOD_TIMEOUT,DayZPlayerConstants.STANCEMASK_ERECT | DayZPlayerConstants.STANCEMASK_CROUCH,false);
						HideItemInHands();
					}
					else
					{
						CreateEmoteCallback(EmoteCB,DayZPlayerConstants.CMD_GESTUREFB_TIMEOUT,DayZPlayerConstants.STANCEMASK_PRONE,true);
						HideItemInHands();
					}
					
				break;
				
				case ID_EMOTE_DABBING:
					CreateEmoteCallback(EmoteCB,DayZPlayerConstants.CMD_GESTUREMOD_DABBING,DayZPlayerConstants.STANCEMASK_ERECT | DayZPlayerConstants.STANCEMASK_CROUCH,false);
					HideItemInHands();
				break;
				
				case ID_EMOTE_FACEPALM:
					CreateEmoteCallback(EmoteCB,DayZPlayerConstants.CMD_GESTUREMOD_FACEPALM,DayZPlayerConstants.STANCEMASK_ERECT | DayZPlayerConstants.STANCEMASK_CROUCH,false);
					//HideItemInHands();
				break;
				
				case ID_EMOTE_CLAP:
					CreateEmoteCallback(EmoteCB,DayZPlayerConstants.CMD_GESTUREMOD_CLAP,DayZPlayerConstants.STANCEMASK_ERECT | DayZPlayerConstants.STANCEMASK_CROUCH,false);
					HideItemInHands();
				break;
				
				case ID_EMOTE_SILENT:
					if (!m_NPC.IsPlayerInStance(DayZPlayerConstants.STANCEMASK_PRONE))
					{
						CreateEmoteCallback(EmoteCB,DayZPlayerConstants.CMD_GESTUREMOD_SILENCE,DayZPlayerConstants.STANCEMASK_ERECT | DayZPlayerConstants.STANCEMASK_CROUCH,false);
						//HideItemInHands();
					}
					else
					{
						CreateEmoteCallback(EmoteCB,DayZPlayerConstants.CMD_GESTUREFB_SILENCE,DayZPlayerConstants.STANCEMASK_PRONE,true);
						//HideItemInHands();
					}
				break;
				
				case ID_EMOTE_WATCHING:
					if (!m_NPC.IsPlayerInStance(DayZPlayerConstants.STANCEMASK_PRONE))
					{
						CreateEmoteCallback(EmoteCB,DayZPlayerConstants.CMD_GESTUREMOD_WATCHING,DayZPlayerConstants.STANCEMASK_CROUCH | DayZPlayerConstants.STANCEMASK_ERECT,false);
					}
					else
					{
						CreateEmoteCallback(EmoteCB,DayZPlayerConstants.CMD_GESTUREFB_WATCHING,DayZPlayerConstants.STANCEMASK_PRONE,true);
					}
				break;
				
				case ID_EMOTE_HOLD:
					if (!m_NPC.IsPlayerInStance(DayZPlayerConstants.STANCEMASK_PRONE))
					{
						CreateEmoteCallback(EmoteCB,DayZPlayerConstants.CMD_GESTUREMOD_HOLD,DayZPlayerConstants.STANCEMASK_CROUCH | DayZPlayerConstants.STANCEMASK_ERECT,false);
					}
					else
					{
						CreateEmoteCallback(EmoteCB,DayZPlayerConstants.CMD_GESTUREFB_HOLD,DayZPlayerConstants.STANCEMASK_PRONE,true);
					}
				break;
				
				case ID_EMOTE_LISTENING:
					if (!m_NPC.IsPlayerInStance(DayZPlayerConstants.STANCEMASK_PRONE))
					{
						CreateEmoteCallback(EmoteCB,DayZPlayerConstants.CMD_GESTUREMOD_LISTENING,DayZPlayerConstants.STANCEMASK_CROUCH | DayZPlayerConstants.STANCEMASK_ERECT,false);
					}
					else
					{
						CreateEmoteCallback(EmoteCB,DayZPlayerConstants.CMD_GESTUREFB_LISTENING,DayZPlayerConstants.STANCEMASK_PRONE,true);
					}
				break;
				
				case ID_EMOTE_POINTSELF:
					if (!m_NPC.IsPlayerInStance(DayZPlayerConstants.STANCEMASK_PRONE))
					{
						CreateEmoteCallback(EmoteCB,DayZPlayerConstants.CMD_GESTUREMOD_POINTSELF,DayZPlayerConstants.STANCEMASK_CROUCH | DayZPlayerConstants.STANCEMASK_ERECT,false);
					}
					else
					{
						CreateEmoteCallback(EmoteCB,DayZPlayerConstants.CMD_GESTUREFB_POINTSELF,DayZPlayerConstants.STANCEMASK_PRONE,true);
					}
				break;
				
				case ID_EMOTE_LOOKATME:
					if (!m_NPC.IsPlayerInStance(DayZPlayerConstants.STANCEMASK_PRONE))
					{
						CreateEmoteCallback(EmoteCB,DayZPlayerConstants.CMD_GESTUREMOD_LOOKATME,DayZPlayerConstants.STANCEMASK_CROUCH | DayZPlayerConstants.STANCEMASK_ERECT,false);
					}
					else
					{
						CreateEmoteCallback(EmoteCB,DayZPlayerConstants.CMD_GESTUREFB_LOOKATME,DayZPlayerConstants.STANCEMASK_PRONE,true);
					}
				break;
				
				case ID_EMOTE_TAUNTTHINK :
					if (!m_NPC.IsPlayerInStance(DayZPlayerConstants.STANCEMASK_PRONE))
					{
						CreateEmoteCallback(EmoteCB,DayZPlayerConstants.CMD_GESTUREMOD_TAUNTTHINK,DayZPlayerConstants.STANCEMASK_CROUCH | DayZPlayerConstants.STANCEMASK_ERECT,false);
					}
					else
					{
						CreateEmoteCallback(EmoteCB,DayZPlayerConstants.CMD_GESTUREFB_TAUNTTHINK,DayZPlayerConstants.STANCEMASK_PRONE,true);
					}
				break;
				
				case ID_EMOTE_MOVE :
					if (!m_NPC.IsPlayerInStance(DayZPlayerConstants.STANCEMASK_PRONE))
					{
						CreateEmoteCallback(EmoteCB,DayZPlayerConstants.CMD_GESTUREMOD_MOVE,DayZPlayerConstants.STANCEMASK_CROUCH | DayZPlayerConstants.STANCEMASK_ERECT,false);
					}
					else
					{
						CreateEmoteCallback(EmoteCB,DayZPlayerConstants.CMD_GESTUREFB_MOVE,DayZPlayerConstants.STANCEMASK_PRONE,true);
					}
				break;
				
				case ID_EMOTE_DOWN :
					if (!m_NPC.IsPlayerInStance(DayZPlayerConstants.STANCEMASK_PRONE))
					{
						CreateEmoteCallback(EmoteCB,DayZPlayerConstants.CMD_GESTUREMOD_DOWN,DayZPlayerConstants.STANCEMASK_CROUCH | DayZPlayerConstants.STANCEMASK_ERECT,false);
					}
					else
					{
						CreateEmoteCallback(EmoteCB,DayZPlayerConstants.CMD_GESTUREFB_DOWN,DayZPlayerConstants.STANCEMASK_PRONE,true);
					}
				break;
				
				case ID_EMOTE_COME :
					if (!m_NPC.IsPlayerInStance(DayZPlayerConstants.STANCEMASK_PRONE))
					{
						CreateEmoteCallback(EmoteCB,DayZPlayerConstants.CMD_GESTUREMOD_COME,DayZPlayerConstants.STANCEMASK_CROUCH | DayZPlayerConstants.STANCEMASK_ERECT,false);
					}
					else
					{
						CreateEmoteCallback(EmoteCB,DayZPlayerConstants.CMD_GESTUREFB_COME,DayZPlayerConstants.STANCEMASK_PRONE,true);
					}
				break;
				
				case ID_EMOTE_CAMPFIRE :
					CreateEmoteCallback(EmoteCB,DayZPlayerConstants.CMD_GESTUREFB_CAMPFIRE,DayZPlayerConstants.STANCEMASK_CROUCH,true);
				break;
				
				case ID_EMOTE_SITA :
					CreateEmoteCallback(EmoteCB,DayZPlayerConstants.CMD_GESTUREFB_SITA,DayZPlayerConstants.STANCEMASK_CROUCH,true);
				break;
				
				case ID_EMOTE_SITB :
					CreateEmoteCallback(EmoteCB,DayZPlayerConstants.CMD_GESTUREFB_SITB,DayZPlayerConstants.STANCEMASK_CROUCH,true);
				break;
				
				case ID_EMOTE_RPS :
					CreateEmoteCallback(EmoteCB,DayZPlayerConstants.CMD_GESTUREMOD_RPS,DayZPlayerConstants.STANCEMASK_ERECT,false);
				break;
				
				case ID_EMOTE_RPS_R :
					CreateEmoteCallback(EmoteCB,DayZPlayerConstants.CMD_GESTUREMOD_RPS,DayZPlayerConstants.STANCEMASK_ERECT,false);
				break;
				
				case ID_EMOTE_RPS_P :
					CreateEmoteCallback(EmoteCB,DayZPlayerConstants.CMD_GESTUREMOD_RPS,DayZPlayerConstants.STANCEMASK_ERECT,false);
				break;
				
				case ID_EMOTE_RPS_S :
					CreateEmoteCallback(EmoteCB,DayZPlayerConstants.CMD_GESTUREMOD_RPS,DayZPlayerConstants.STANCEMASK_ERECT,false);
				break;
				
				//new stuff
				case ID_EMOTE_NOD :
					if (!m_NPC.IsPlayerInStance(DayZPlayerConstants.STANCEMASK_PRONE))
					{
						CreateEmoteCallback(EmoteCB,DayZPlayerConstants.CMD_GESTUREMOD_NODHEAD,DayZPlayerConstants.STANCEMASK_CROUCH | DayZPlayerConstants.STANCEMASK_ERECT,false);
					}
					else
					{
						CreateEmoteCallback(EmoteCB,DayZPlayerConstants.CMD_GESTUREFB_NODHEAD,DayZPlayerConstants.STANCEMASK_PRONE,true);
					}
				break;
				
				case ID_EMOTE_SHAKE :
					if (!m_NPC.IsPlayerInStance(DayZPlayerConstants.STANCEMASK_PRONE))
					{
						CreateEmoteCallback(EmoteCB,DayZPlayerConstants.CMD_GESTUREMOD_SHAKEHEAD,DayZPlayerConstants.STANCEMASK_CROUCH | DayZPlayerConstants.STANCEMASK_ERECT,false);
					}
					else
					{
						CreateEmoteCallback(EmoteCB,DayZPlayerConstants.CMD_GESTUREFB_SHAKEHEAD,DayZPlayerConstants.STANCEMASK_PRONE,true);
					}
				break;
				
				case ID_EMOTE_SHRUG :
					if (!m_NPC.IsPlayerInStance(DayZPlayerConstants.STANCEMASK_PRONE))
					{
						CreateEmoteCallback(EmoteCB,DayZPlayerConstants.CMD_GESTUREMOD_SHRUG,DayZPlayerConstants.STANCEMASK_CROUCH | DayZPlayerConstants.STANCEMASK_ERECT,false);
					}
					else
					{
						CreateEmoteCallback(EmoteCB,DayZPlayerConstants.CMD_GESTUREFB_SHRUG,DayZPlayerConstants.STANCEMASK_PRONE,true);
					}
				break;
				
				default :
					Print("EmoteManager.c | PlayEmote | WRONG ID");
					m_bEmoteIsPlaying = false;
					return false;
				break;
			}
		}
		if ( m_bEmoteIsPlaying )
		{
			if ( m_NPC.GetActionManager() )	 m_NPC.GetActionManager().DisableActions();
			//m_NPC.GetInventory().LockInventory(LOCK_FROM_SCRIPT);
			m_NPC.m_InventorySoftLocked = true;
		}
		
		return true;
	}

    //creates Emote callback
	protected void CreateEmoteCallback(typename callbacktype, int id, int mask, bool fullbody )
	{
		if ( m_NPC )
		{
			m_LastMask = -1;
			if ( fullbody )
			{
				Class.CastTo(m_Callback, m_NPC.StartCommand_Action(id,callbacktype,mask));
				m_Callback.m_IsFullbody = true;
				m_Callback.EnableCancelCondition(true);
			}
			else if (m_NPC.IsPlayerInStance(mask))
			{
				m_LastMask = mask;  //character is probably not prone now
				Class.CastTo(m_Callback, m_NPC.AddCommandModifier_Action(id,callbacktype));
			}
			
			if (m_Callback)
			{
				m_bEmoteIsPlaying = true;
				m_Callback.m_NPC = m_NPC;
			}
		}
	}
	
	protected void HideItemInHands()
	{
		m_item = m_NPC.GetItemInHands();
		if(m_Callback && m_item)
		{
			m_ItemIsOn = m_item.IsPilotLight();
			if (m_ItemIsOn)
				m_item.SetPilotLight(false);
			m_item.SetInvisible(true);
			m_ItemToHands = true;
		}
	}
	
	protected void ShowItemInHands()
	{
		if (m_item)
		{
			if (m_ItemIsOn)
			{
				m_item.SetPilotLight(m_ItemIsOn);
				m_ItemIsOn = false;
			}
			m_item.SetInvisible(false);
			m_NPC.GetItemAccessor().OnItemInHandsChanged();
		}
		m_ItemToHands = false;
	}
	
	protected void CommitSuicide()
	{
		Weapon_Base weapon;
		WeaponEventBase weapon_event = new WeaponEventTrigger;
		
		//firearm suicide
		if ( Weapon_Base.CastTo(weapon,m_NPC.GetItemInHands()) )
		{
			//TODO : check multiple muzzles for shotguns, eventually
			if (weapon.CanFire())
			{
				m_NPC.SetSuicide(true);
				weapon.ProcessWeaponEvent(weapon_event);
				m_Callback.InternalCommand(DayZPlayerConstants.CMD_ACTIONINT_END2);
				if (m_NPC.IsAlive()) 	(GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).Call(this.KillPlayer);
			}
			else
			{
				weapon.ProcessWeaponEvent(weapon_event);
				m_Callback.InternalCommand(DayZPlayerConstants.CMD_ACTIONINT_END);
			}
		}
		//melee weapon suicide
		else if ( m_NPC.GetItemInHands() && m_NPC.GetItemInHands().ConfigIsExisting("suicideAnim") )
		{
			m_Callback.RegisterAnimationEvent("Death",1);
			m_NPC.SetSuicide(true);
			m_Callback.InternalCommand(DayZPlayerConstants.CMD_ACTIONINT_END2);
			//GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(this.KillPlayer, 4000, false);
		}
		//unarmed "suicide" :)
		else
		{
			m_Callback.InternalCommand(DayZPlayerConstants.CMD_ACTIONINT_END2);
		}
	}

	void KillPlayer()
	{
		if (GetGame().IsServer())
		{
			m_NPC.SetHealth(0);
		}
	}

    void EndCallback()
	{
		if (!m_Callback)
		{
			//Print("no callback!");
			return;
		}
		
		ScriptInputUserData ctx = new ScriptInputUserData;
		if (GetGame().IsMultiplayer() && GetGame().IsClient() && !ctx.CanStoreInputUserData())
		{
			//Print("ctx unavailable");
			return;
		}
		else if (GetGame().IsMultiplayer() && GetGame().IsClient() && ctx.CanStoreInputUserData())
		{
			//Print("sending ECB cancel request");
			ctx.Write(INPUT_UDT_GESTURE);
			ctx.Write(-1);
			ctx.Send();
		}
		
		//Print("ending ECB - proper");
		if (m_CurrentGestureID == ID_EMOTE_DANCE)
		{
			m_Callback.InternalCommand(DayZPlayerConstants.CMD_ACTIONINT_INTERRUPT);
		}
		else
		{
			m_Callback.InternalCommand(DayZPlayerConstants.CMD_ACTIONINT_END);
		}
	}

    bool CanPlayEmote()
	{
		if ( !m_NPC || m_NPC.GetCommand_Action() || m_NPC.GetCommandModifier_Action() )
		{	
			return false;
		}
		if ( m_NPC.GetWeaponManager().IsRunning() || m_NPC.GetActionManager().GetRunningAction() )
		{
			return false;
		}
		
		if ( m_NPC.IsFighting() || m_NPC.IsSwimming() || m_NPC.IsClimbingLadder() || m_NPC.IsFalling() || m_NPC.IsUnconscious() ) 	//TODO rework conditions into something better?
		{
			return false;
		}
		if (m_NPC.GetCommand_Vehicle())
		{
			if (m_NPC.GetCommand_Vehicle().GetTransport())
			{
				return false;
			}
		}
		return true;
	}
}
