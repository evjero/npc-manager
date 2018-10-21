class NPCWeaponManager
{
    protected NPCBase m_npc;
    protected bool						m_canEnd;
	protected bool						m_justStart;
	protected bool						m_InProgress;
	protected bool						m_IsEventSended;
	protected Weapon_Base				m_WeaponInHand;

    void NPCWeaponManager(NPCBase npc)
    {
        m_npc = npc;
    }
    bool IsRunning()
	{
		return m_InProgress;
	}

    void SetRunning( bool value)
	{
		m_InProgress = value;
	}

    void Update( float deltaT )
	{
		if( !m_InProgress || !m_IsEventSended )
			return;
		
		if(m_canEnd)
		{
			
			if(m_WeaponInHand.IsIdle())
			{
				OnWeaponActionEnd();
			}
			else if(m_justStart)
			{				
				m_justStart = false;
			}
			
		}
		else
		{
			m_canEnd = true;
			m_justStart = true;
		}
		
	}
}