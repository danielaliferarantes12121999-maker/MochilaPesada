//------------------------------------------------------------------------------------------------
// Global base-captured event and manager hooks.
//------------------------------------------------------------------------------------------------
void OnBaseCaptureDelegate(SCR_CampaignMilitaryBaseComponent capturedBase);
typedef func OnBaseCaptureDelegate;
typedef ScriptInvokerBase<OnBaseCaptureDelegate> OnBaseCaptureInvoker;

modded class SCR_CampaignMilitaryBaseManager
{
	protected ref OnBaseCaptureInvoker m_OnBaseCaptured;

	OnBaseCaptureInvoker GetOnBaseCaptured()
	{
		if (!m_OnBaseCaptured)
			m_OnBaseCaptured = new OnBaseCaptureInvoker();

		return m_OnBaseCaptured;
	}

	override void OnAllBasesInitialized()
	{
		super.OnAllBasesInitialized();

		array<SCR_MilitaryBaseComponent> allBases = {};
		SCR_MilitaryBaseSystem.GetInstance().GetBases(allBases);
		Narco_CacheSystem.BuildCache(allBases);

		SCR_RadioCoverageSystem.Narco_InitAndStartUpdates();
	}

	override protected void OnBaseFactionChanged(SCR_MilitaryBaseComponent base, Faction newFaction)
	{
		super.OnBaseFactionChanged(base, newFaction);

		if (m_Campaign.IsProxy())
			return;

		foreach (SCR_CampaignMilitaryBaseComponent baseToCheck : m_aBases)
		{
			if (!baseToCheck)
				continue;

			Faction baseCapturingFaction = baseToCheck.GetCapturingFaction();
			if (!baseCapturingFaction)
				continue;

			SCR_CampaignFaction capturingFaction = SCR_CampaignFaction.Cast(baseCapturingFaction);
			if (!capturingFaction)
				continue;

			if (!baseToCheck.IsHQRadioTrafficPossible(capturingFaction))
				baseToCheck.EndCapture();
		}
	}
};

modded class SCR_CampaignMilitaryBaseComponent
{
	override protected void OnFactionChanged(FactionAffiliationComponent owner, Faction previousFaction, Faction faction)
	{
		super.OnFactionChanged(owner, previousFaction, faction);

		SCR_GameModeCampaign campaign = SCR_GameModeCampaign.GetInstance();
		if (!campaign || !campaign.GetBaseManager())
			return;

		campaign.GetBaseManager().GetOnBaseCaptured().Invoke(this);
	}

	override void EvaluateDefenders()
	{
		if (m_bIsHQ)
		{
			GetGame().GetCallqueue().Remove(EvaluateDefenders);
			return;
		}

		super.EvaluateDefenders();
	}
};
