//------------------------------------------------------------------------------------------------
// Hotfix for spawn point map UI null access during teardown/update race.
//------------------------------------------------------------------------------------------------
modded class SCR_MapUISpawnPoint
{
	override vector GetPos()
	{
		if (!m_SpawnPoint)
			return vector.Zero;

		return super.GetPos();
	}
};
