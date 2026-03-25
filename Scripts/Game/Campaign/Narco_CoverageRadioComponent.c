modded class SCR_CoverageRadioComponent
{
	// Helper to allow direct-graph reconciliation to update signal state.
	void Narco_SetCoverage(string encryptionKey)
	{
		if (!m_aEncryptionKeyCoverSend.Contains(encryptionKey))
			m_aEncryptionKeyCoverSend.Insert(encryptionKey);

		if (!m_aEncryptionKeyCoverReceive.Contains(encryptionKey))
			m_aEncryptionKeyCoverReceive.Insert(encryptionKey);
	}
};
