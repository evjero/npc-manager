class NPCBase extends ManBase
{
    private int m_BloodType;
    
	ref protected ActionManagerBase	m_ActionManager;
    ref EmoteManager m_EmoteManager;
	ref StanceIndicator m_StanceIndicator;
    ref WeaponManager m_WeaponManager;
    ref Bot	m_Bot;

    void NPCBase()
    {
        Init();
    }

    void Init()
    {
		SetEventMask(EntityEvent.INIT|EntityEvent.POSTFRAME);
		m_BloodType = 0;
        m_AnalyticsTimer = new Timer( CALL_CATEGORY_SYSTEM );

		if(  GetGame().IsClient() || ( !GetGame().IsMultiplayer() && GetGame().IsServer() ) )
		{
			m_StanceIndicator = new StanceIndicator(this);
		}
        
        m_EmoteManager = new NPCEmoteManager(this);
        m_WeaponManager = new WeaponManager(this);
    }
}