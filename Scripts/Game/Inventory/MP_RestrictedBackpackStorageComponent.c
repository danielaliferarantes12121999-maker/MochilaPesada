// ====================================================================================
// MochilaPesada - Inventory restriction system (preventive block before transfer)
// ====================================================================================

[BaseContainerProps(configRoot: true)]
class MP_ContainerRestrictionRule
{
	// ID logical do container. Ex.: "backpack"
	[Attribute("", UIWidgets.EditBox)]
	string m_sContainerKey;

	// Prefabs bloqueados neste container
	[Attribute("", UIWidgets.Object)]
	ref array<ResourceName> m_aBlockedPrefabs;

	void MP_ContainerRestrictionRule()
	{
		if (!m_aBlockedPrefabs)
			m_aBlockedPrefabs = {};
	}
};

[BaseContainerProps(configRoot: true)]
class MP_InventoryRestrictionConfig
{
	[Attribute(desc: "Lista de regras de bloqueio por container")]
	ref array<ref MP_ContainerRestrictionRule> m_aRules;

	void MP_InventoryRestrictionConfig()
	{
		if (!m_aRules)
			m_aRules = {};
	}
};

// Banco estático para facilitar extensão futura (outros containers + itens)
class MP_InventoryRestrictionDB
{
	protected static ref map<string, ref set<ResourceName>> s_mBlockedByContainer;
	protected static bool s_bInitialized;

	static void EnsureInit()
	{
		if (s_bInitialized)
			return;

		s_bInitialized = true;
		s_mBlockedByContainer = new map<string, ref set<ResourceName>>();

		// Exemplo mínimo já funcional:
		// ajuste os ResourceName abaixo para os prefabs exatos da(s) munição(ões) RPG do seu mod/jogo.
		RegisterBlockedItem("backpack", "{B3BFAB4E74D2DA76}Prefabs/Weapons/Launchers/RPG/RPG7_Ammo_HEAT.et");
		RegisterBlockedItem("backpack", "{5A01D4C44F996D09}Prefabs/Weapons/Launchers/RPG/RPG7_Ammo_OG7V.et");
	}

	static void RegisterBlockedItem(string containerKey, ResourceName prefab)
	{
		ref set<ResourceName> blocked;
		if (!s_mBlockedByContainer.Find(containerKey, blocked))
		{
			blocked = new set<ResourceName>();
			s_mBlockedByContainer.Set(containerKey, blocked);
		}

		blocked.Insert(prefab);
	}

	static bool IsBlocked(string containerKey, ResourceName prefab)
	{
		EnsureInit();

		ref set<ResourceName> blocked;
		if (!s_mBlockedByContainer.Find(containerKey, blocked))
			return false;

		return blocked.Contains(prefab);
	}
};

// Componente para substituir/apendar no storage da mochila
// Faz bloqueio PREVENTIVO no CanStoreItem (antes da transferência)
class MP_RestrictedBackpackStorageComponentClass: SCR_UniversalInventoryStorageComponentClass {}
class MP_RestrictedBackpackStorageComponent: SCR_UniversalInventoryStorageComponent
{
	// A chave permite reutilização para outros tipos de container no futuro.
	[Attribute("backpack", UIWidgets.EditBox)]
	protected string m_sContainerKey;

	override bool OnOverrideCanStoreItem()
	{
		// informa ao sistema que este componente quer decidir dinamicamente CanStoreItem.
		return true;
	}

	override bool CanStoreItem(IEntity item)
	{
		// Primeiro mantém todas as validações nativas do jogo.
		if (!super.CanStoreItem(item))
			return false;

		ResourceName prefab = MP_RestrictedBackpackStorageComponent.GetItemPrefab(item);
		if (!prefab || prefab == string.Empty)
			return true;

		// Se for item bloqueado para este container, impede imediatamente.
		if (MP_InventoryRestrictionDB.IsBlocked(m_sContainerKey, prefab))
			return false;

		return true;
	}

	protected static ResourceName GetItemPrefab(IEntity item)
	{
		if (!item)
			return string.Empty;

		IEntitySource src = item.GetSource();
		if (!src)
			return string.Empty;

		return src.GetPrefabName();
	}
};
