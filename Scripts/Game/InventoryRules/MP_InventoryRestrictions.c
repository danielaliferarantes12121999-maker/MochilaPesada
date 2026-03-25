// ====================================================================================
// MochilaPesada - New independent mod
// Rule A) Block RPG ammo into backpack (preventive)
// Rule B) Max 5 grenades in player's inventory (preventive)
// ====================================================================================

class MP_InventoryRestrictionUtils
{
	// -----------------------------------------------------------------------------
	// WHERE: item -> RPG ammo detection
	// -----------------------------------------------------------------------------
	static bool IsRpgAmmo(IEntity item)
	{
		if (!item)
			return false;

		IEntitySource src = item.GetSource();
		if (!src)
			return false;

		ResourceName prefab = src.GetPrefabName();
		if (prefab == string.Empty)
			return false;

		string prefabLower = prefab;
		prefabLower.ToLower();

		// Safer default heuristic (you can replace with exact prefab whitelist)
		return prefabLower.Contains("rpg") && prefabLower.Contains("ammo");
	}

	// -----------------------------------------------------------------------------
	// WHERE: item -> grenade detection
	// -----------------------------------------------------------------------------
	static bool IsGrenade(IEntity item)
	{
		if (!item)
			return false;

		IEntitySource src = item.GetSource();
		if (!src)
			return false;

		ResourceName prefab = src.GetPrefabName();
		if (prefab == string.Empty)
			return false;

		string prefabLower = prefab;
		prefabLower.ToLower();

		// Generic fallback. For production, prefer exact whitelist by prefab ResourceName.
		return prefabLower.Contains("grenade");
	}

	// -----------------------------------------------------------------------------
	// WHERE: count grenades currently owned by player
	// -----------------------------------------------------------------------------
	static int CountGrenadesInPlayerInventory(SCR_InventoryStorageManagerComponent manager)
	{
		if (!manager)
			return 0;

		array<IEntity> allItems = {};
		manager.GetItems(allItems);

		int grenadeCount = 0;
		foreach (IEntity item : allItems)
		{
			if (IsGrenade(item))
				grenadeCount++;
		}

		return grenadeCount;
	}

	// Helps avoid blocking internal reordering when player already has 5 grenades.
	static bool IsAlreadyInsidePlayerInventory(IEntity item, IEntity player)
	{
		if (!item || !player)
			return false;

		IEntity root = item.GetRootParent();
		return root == player;
	}
};

// ====================================================================================
// Rule A - backpack destination guard
// Attach this component to backpack storage prefab (destination container).
// ====================================================================================
class MP_BackpackRestrictedStorageComponentClass : SCR_UniversalInventoryStorageComponentClass {}
class MP_BackpackRestrictedStorageComponent : SCR_UniversalInventoryStorageComponent
{
	override bool OnOverrideCanStoreItem()
	{
		// Enables preventive decision on transfer attempt.
		return true;
	}

	override bool CanStoreItem(IEntity item)
	{
		if (!super.CanStoreItem(item))
			return false;

		// -------------------------------------------------------------------------
		// WHERE: block RPG ammo when destination is backpack
		// -------------------------------------------------------------------------
		if (MP_InventoryRestrictionUtils.IsRpgAmmo(item))
			return false;

		return true;
	}
};

// ====================================================================================
// Rule B - global grenade cap for player inventory
// Modded directly in player inventory storage for broad coverage
// (drag/drop, move between inventories, pick from ground/arsenal/container).
// ====================================================================================
modded class SCR_CharacterInventoryStorageComponent
{
	[Attribute("5", UIWidgets.EditBox, desc: "Maximum grenades the player can carry")]
	protected int m_iMaxGrenades;

	override bool OnOverrideCanStoreItem()
	{
		return true;
	}

	override bool CanStoreItem(IEntity item)
	{
		if (!super.CanStoreItem(item))
			return false;

		if (!MP_InventoryRestrictionUtils.IsGrenade(item))
			return true;

		IEntity player = GetOwner();
		if (MP_InventoryRestrictionUtils.IsAlreadyInsidePlayerInventory(item, player))
			return true;

		SCR_InventoryStorageManagerComponent invManager = SCR_InventoryStorageManagerComponent.Cast(player.FindComponent(SCR_InventoryStorageManagerComponent));
		if (!invManager)
			return true;

		int currentGrenades = MP_InventoryRestrictionUtils.CountGrenadesInPlayerInventory(invManager);

		// -------------------------------------------------------------------------
		// WHERE: deny picking/moving the 6th grenade (preventive)
		// -------------------------------------------------------------------------
		if (currentGrenades >= m_iMaxGrenades)
			return false;

		return true;
	}
};
