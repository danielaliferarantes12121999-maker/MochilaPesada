//------------------------------------------------------------------------------------------------
// Central cache for direct-connect base lookup by entity name.
//------------------------------------------------------------------------------------------------
class Narco_CacheSystem
{
	protected static ref map<string, Narco_DirectConnectBaseComponent> s_mBaseCache;

	static void BuildCache(notnull array<SCR_MilitaryBaseComponent> allBases)
	{
		s_mBaseCache = new map<string, Narco_DirectConnectBaseComponent>();

		foreach (SCR_MilitaryBaseComponent baseComp : allBases)
		{
			if (!baseComp)
				continue;

			Narco_DirectConnectBaseComponent directBase = Narco_DirectConnectBaseComponent.Cast(baseComp);
			if (!directBase)
				continue;

			IEntity owner = directBase.GetOwner();
			if (!owner)
				continue;

			string baseName = owner.GetName();
			if (baseName.IsEmpty())
				continue;

			s_mBaseCache.Set(baseName, directBase);
		}
	}

	static Narco_DirectConnectBaseComponent GetBaseByName(string name)
	{
		if (!s_mBaseCache || name.IsEmpty())
			return null;

		return s_mBaseCache.Get(name);
	}

	static map<string, Narco_DirectConnectBaseComponent> GetBaseCache()
	{
		if (!s_mBaseCache)
			s_mBaseCache = new map<string, Narco_DirectConnectBaseComponent>();

		return s_mBaseCache;
	}
};
