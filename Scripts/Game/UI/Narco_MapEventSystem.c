//------------------------------------------------------------------------------------------------
// Refresh map modules and base descriptors when a base is captured while map is open.
//------------------------------------------------------------------------------------------------
modded class SCR_MapEntity
{
	override protected void OnMapOpen(MapConfiguration config)
	{
		super.OnMapOpen(config);

		SCR_GameModeCampaign campaign = SCR_GameModeCampaign.GetInstance();
		if (campaign && campaign.GetBaseManager())
			campaign.GetBaseManager().GetOnBaseCaptured().Insert(OnGlobalBaseCaptured);
	}

	override protected void OnMapClose()
	{
		SCR_GameModeCampaign campaign = SCR_GameModeCampaign.GetInstance();
		if (campaign && campaign.GetBaseManager())
			campaign.GetBaseManager().GetOnBaseCaptured().Remove(OnGlobalBaseCaptured);

		super.OnMapClose();
	}

	protected void OnGlobalBaseCaptured(SCR_CampaignMilitaryBaseComponent capturedBase)
	{
		SCR_RadioCoverageSystem.UpdateAll(true);

		Cleanup();
		ActivateModules(m_ActiveMapCfg.Modules);
		ActivateComponents(m_ActiveMapCfg.Components);
		ActivateOtherComponents(m_ActiveMapCfg.OtherComponents);

		array<SCR_MilitaryBaseComponent> bases = {};
		SCR_MilitaryBaseSystem.GetInstance().GetBases(bases);

		foreach (SCR_MilitaryBaseComponent baseComp : bases)
		{
			SCR_CampaignMilitaryBaseComponent campaignBase = SCR_CampaignMilitaryBaseComponent.Cast(baseComp);
			if (!campaignBase || !campaignBase.IsInitialized())
				continue;

			SCR_CampaignMilitaryBaseMapDescriptorComponent mapDescriptor = campaignBase.GetMapDescriptor();
			if (mapDescriptor)
				mapDescriptor.HandleMapInfo();

			SCR_CampaignMapUIBase uiElement = campaignBase.GetMapUI();
			if (uiElement)
				uiElement.SetIconInfoText();
		}
	}
};
