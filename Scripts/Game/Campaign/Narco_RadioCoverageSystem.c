modded class SCR_RadioCoverageSystem
{
	protected static bool s_bNarcoSystemReady = false;

	override void OnStarted()
	{
		// Intentionally empty: initialization is manually triggered via Narco_InitAndStartUpdates.
	}

	override void OnStopped()
	{
		GetGame().GetCallqueue().Remove(Narco_PeriodicUpdate);
	}

	void Narco_PeriodicUpdate()
	{
		UpdateAll(false);
	}

	static void Narco_InitAndStartUpdates()
	{
		if (s_bNarcoSystemReady)
			return;

		s_bNarcoSystemReady = true;

		UpdateAll(true);

		SCR_RadioCoverageSystem instance = GetInstance();
		if (instance)
			GetGame().GetCallqueue().CallLater(instance.Narco_PeriodicUpdate, UPDATE_DELAY, true);
	}

	override static bool UpdateAll(bool forceRecalculation = false)
	{
		if (!s_bNarcoSystemReady)
			return false;

		Narco_ReconcileAllConnections();
		return true;
	}

	protected static void Narco_CustomPing(SCR_CoverageRadioComponent currentRadio, string encryptionKey, inout array<SCR_CoverageRadioComponent> visited)
	{
		if (!currentRadio || visited.Contains(currentRadio))
			return;

		visited.Insert(currentRadio);
		currentRadio.Narco_SetCoverage(encryptionKey);

		array<SCR_CoverageRadioComponent> neighbors = {};
		currentRadio.GetRadiosInRange(neighbors);

		foreach (SCR_CoverageRadioComponent neighbor : neighbors)
			Narco_CustomPing(neighbor, encryptionKey, visited);
	}

	static void Narco_ReconcileAllConnections()
	{
		SCR_RadioCoverageSystem rcs = GetInstance();
		if (!rcs)
			return;

		array<SCR_CoverageRadioComponent> allRadios = {};
		rcs.GetRadios(allRadios);
		if (allRadios.IsEmpty())
			return;

		foreach (SCR_CoverageRadioComponent radio : allRadios)
		{
			array<SCR_CoverageRadioComponent> existingLinks = {};
			radio.GetRadiosInRange(existingLinks);

			foreach (SCR_CoverageRadioComponent linkedRadio : existingLinks)
			{
				radio.RemoveRadioInRange(linkedRadio);
				radio.RemoveRadioInRangeOf(linkedRadio);
			}
		}

		foreach (SCR_CoverageRadioComponent radioA : allRadios)
		{
			Narco_DirectConnectBaseComponent baseA = Narco_DirectConnectBaseComponent.Cast(radioA.GetOwner().FindComponent(Narco_DirectConnectBaseComponent));
			if (!baseA)
				continue;

			array<string> desiredConnectionNames = {};
			baseA.GetNextCapturableBaseNames(desiredConnectionNames);

			foreach (string targetName : desiredConnectionNames)
			{
				Narco_DirectConnectBaseComponent baseB = Narco_CacheSystem.GetBaseByName(targetName);
				if (!baseB)
					continue;

				SCR_CoverageRadioComponent radioB = SCR_CoverageRadioComponent.Cast(baseB.GetOwner().FindComponent(SCR_CoverageRadioComponent));
				if (!radioB)
					continue;

				radioA.AddRadioInRange(radioB);
				radioB.AddRadioInRange(radioA);
				radioA.AddRadioInRangeOf(radioB);
				radioB.AddRadioInRangeOf(radioA);
			}
		}

		map<string, SCR_CoverageRadioComponent> factionHQs = new map<string, SCR_CoverageRadioComponent>();
		map<string, Narco_DirectConnectBaseComponent> baseCache = Narco_CacheSystem.GetBaseCache();

		foreach (string baseName, Narco_DirectConnectBaseComponent baseComp : baseCache)
		{
			if (!baseComp)
				continue;

			if (!baseComp.IsHQ())
				continue;

			SCR_CoverageRadioComponent radio = SCR_CoverageRadioComponent.Cast(baseComp.GetOwner().FindComponent(SCR_CoverageRadioComponent));
			if (radio)
				factionHQs.Insert(radio.GetEncryptionKey(), radio);
		}

		bool signalSignature;
		for (int i = 0; i < factionHQs.Count(); i++)
		{
			string encryptionKey = factionHQs.GetKey(i);
			SCR_CoverageRadioComponent source = factionHQs.GetElement(i);

			foreach (SCR_CoverageRadioComponent radio : allRadios)
				radio.PrepareCoverageUpdate(encryptionKey, signalSignature);

			signalSignature = !signalSignature;

			array<SCR_CoverageRadioComponent> visitedRadios = {};
			Narco_CustomPing(source, encryptionKey, visitedRadios);

			foreach (SCR_CoverageRadioComponent radioFinish : allRadios)
				radioFinish.FinishCoverageUpdate(encryptionKey);
		}
	}

	void GetRadios(inout array<SCR_CoverageRadioComponent> outRadios)
	{
		outRadios.Copy(m_aRadioComponents);
	}
};
