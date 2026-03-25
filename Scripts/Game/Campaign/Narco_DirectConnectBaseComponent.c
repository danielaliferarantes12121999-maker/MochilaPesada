//------------------------------------------------------------------------------------------------
// Direct connection model for campaign bases.
//------------------------------------------------------------------------------------------------
[BaseContainerProps()]
class Narco_DirectConnectBaseComponentClass : SCR_CampaignMilitaryBaseComponentClass
{
}

class Narco_DirectConnectBaseComponent : SCR_CampaignMilitaryBaseComponent
{
	[Attribute(desc: "List of base ENTITY NAMES that become capturable after this one is taken.", category: "Campaign")]
	protected ref array<string> m_aNextCapturableBaseNames;

	protected bool IsConnectedTo(Narco_DirectConnectBaseComponent otherBase)
	{
		if (!otherBase || !m_aNextCapturableBaseNames)
			return false;

		IEntity otherOwner = otherBase.GetOwner();
		if (!otherOwner)
			return false;

		string otherBaseName = otherOwner.GetName();
		if (otherBaseName.IsEmpty())
			return false;

		return m_aNextCapturableBaseNames.Contains(otherBaseName);
	}

	override bool IsHQRadioTrafficPossible(notnull SCR_CampaignFaction faction, SCR_ERadioCoverageStatus direction = SCR_ERadioCoverageStatus.RECEIVE)
	{
		if (IsHQ() && GetCampaignFaction() == faction)
			return true;

		SCR_MilitaryBaseSystem baseSystem = SCR_MilitaryBaseSystem.GetInstance();
		if (!baseSystem)
			return false;

		array<SCR_MilitaryBaseComponent> allBases = {};
		baseSystem.GetBases(allBases);

		foreach (SCR_MilitaryBaseComponent friendlyBaseComp : allBases)
		{
			if (!friendlyBaseComp || friendlyBaseComp.GetFaction() != faction)
				continue;

			Narco_DirectConnectBaseComponent friendlyModdedBase = Narco_DirectConnectBaseComponent.Cast(friendlyBaseComp);
			if (friendlyModdedBase && friendlyModdedBase.IsConnectedTo(this))
				return true;
		}

		return false;
	}

	void GetNextCapturableBaseNames(out array<string> outNames)
	{
		if (!outNames)
			outNames = {};

		if (m_aNextCapturableBaseNames)
			outNames.Copy(m_aNextCapturableBaseNames);
	}
};
