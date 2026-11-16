#pragma once
// Reconstructed from patterns.json / sub_1803A2870 / strings
// Inventory & skins system: items, skins, stickers, charms, pets.

#include <cstdint>
#include <string>
#include <vector>
#include <unordered_map>

namespace inventory {

// ─── Item definition (from pattern targets: item_view_ctor/init/name) ─────
struct ItemDefinition {
    int      definition_index;
    int      item_id;
    int      rarity;
    int      quality;
    std::string name;
    std::string model_name;
    int      paint_kit;
    int      seed;
    float    wear;
    int      stattrak;
    std::string type;         // "rifle", "pistol", "knife", "glove", etc.
};

// ─── Weapon type (from sub_1803A2870: m_WeaponType) ──────────────────────
enum class WeaponType {
    Unknown = 0,
    Pistol,
    SMG,
    Rifle,
    Sniper,
    Shotgun,
    Machinegun,
    Grenade,
    Knife,
    Gloves,
    C4,
    Healthshot
};

// ─── Equipment slot (from pattern: slot_of_def) ────────────────────────────
enum class EquipSlot {
    Primary   = 0,
    Secondary = 1,
    Knife     = 2,
    Grenade   = 3,
    C4        = 4,
    Head      = 5,
    Chest     = 6,
    Hands     = 7,
    Legs      = 8,
    Feet      = 9
};

// ─── Sticker (from pattern: apply_sticker_site) ────────────────────────────
struct Sticker {
    int      sticker_id;
    float    wear;
    float    rotation;
    float    scale;
    float    offset_x;
    float    offset_y;
    int      slot;         // 0-4
    float    tint;
};

// ─── Charm (from pattern: place_charm_site) ────────────────────────────────
struct Charm {
    int      charm_id;
    int      slot;
    float    position;
    float    scale;
};

// ─── Skin config (from config_tree skins.*) ────────────────────────────────
struct SkinConfig {
    bool     enabled;
    int      paint_kit;
    int      seed;
    float    wear;
    int      stattrak;
    std::string name;

    // Override
    std::vector<Sticker> stickers;
    Charm    charm;

    // Model override
    int      model_index;
    std::string model_path;
};

// ─── Inventory item (runtime) ──────────────────────────────────────────────
struct InventoryItem {
    uint64_t   item_id;           // CEconItem
    int        definition_index;
    int        paint_kit;
    int        seed;
    float      wear;
    int        stattrak;
    int        quality;
    int        rarity;
    int        origin;
    int        inventory;
    EquipSlot  slot;
    std::vector<Sticker> stickers;
    Charm      charm;
    std::string custom_name;
};

// ─── Apply skin entry (from sub_1803A2870) ────────────────────────────────
struct ApplySkinRequest {
    void*         weapon_entity;
    int           definition_index;
    int           paint_kit;
    int           seed;
    float         wear;
    int           stattrak;
    std::vector<Sticker> stickers;
    Charm         charm;
    std::string   custom_name;
};

// ─── Inventory manager ─────────────────────────────────────────────────────
class InventoryManager {
public:
    static InventoryManager& Instance() { static InventoryManager m; return m; }

    // ─── Pattern targets (from patterns.json) ─────────────────────────────
    // inventory_manager      — CInventoryManager
    // local_inventory        — local player inventory
    // create_reference_item  — create item for preview
    // register_preview_item  — register preview
    // econ_item_create       — CEconItem creation
    // find_type_cache        — type cache
    // slot_of_def            — slot for definition
    // can_equip              — can equip check
    // attribute_by_name      — get attribute
    // attribute_set_uint     — set uint attribute
    // attribute_set_float    — set float attribute
    // equip_site             — equip to site
    // patch_proxy            — patch proxy
    // impact_box_site        — sticker position
    // apply_sticker_site     — apply sticker
    // place_charm_site       — place charm
    // tool_slot_site         — tool slot
    // item_view_ctor/init/name/name
    // item_definition_by_index
    // icon_generator/update/post/finish/targets

    // ─── Apply skin to weapon ────────────────────────────────────────────
    bool ApplySkin(const ApplySkinRequest& request) {
        // 1. Create CEconItem (econ_item_create)
        // 2. Set attributes (attribute_set_uint/float):
        //    "paint kit id" = request.paint_kit
        //    "pattern seed" = request.seed
        //    "paint wear"   = request.wear
        //    "kill score"   = request.stattrak
        //    "custom name"  = request.custom_name
        // 3. For each sticker: apply_sticker_site
        // 4. For charm: place_charm_site
        // 5. Equip to slot
        return false;
    }

    // ─── Preview item (from patterns: preview_*) ──────────────────────────
    void* PreviewItem(int definition_index, int paint_kit) {
        // create_reference_item + register_preview_item
        return nullptr;
    }

    // ─── Get items by slot ───────────────────────────────────────────────
    std::vector<InventoryItem> GetItemsBySlot(EquipSlot slot) {
        std::vector<InventoryItem> result;
        // Iterate local_inventory
        return result;
    }

    // ─── Get all items ────────────────────────────────────────────────────
    std::vector<InventoryItem> GetAllItems() {
        std::vector<InventoryItem> result;
        return result;
    }

    // ─── Model override (for knives) ──────────────────────────────────────
    bool SetModelOverride(void* weapon, const std::string& model_path) {
        // From patterns: find_item_view, chicken_update_model
        return false;
    }

private:
    std::unordered_map<int, SkinConfig> skin_configs_;
};

// ─── Pet system (from patterns: pet_*) ────────────────────────────────────
struct Pet {
    uint64_t   item_id;
    int        growth_level;
    int        feed_level;
    int        lifetime;
    int        food;
    bool       hungry;
    int        upgrade_level;
};

class PetManager {
public:
    static PetManager& Instance() { static PetManager m; return m; }

    // Pattern targets: pet_item_id, pet_growth, pet_feed,
    // pet_lifetime, pet_low_food, pet_upgrade_level, pet_anim_event

    void UpdatePets() {
        // Called every frame (hook: pet_anim_event)
    }

    void Feed(int pet_index) {
        // pet_feed
    }

    void Upgrade(int pet_index) {
        // pet_upgrade_level
    }

private:
    std::vector<Pet> pets_;
};

// ─── Chicken system (from patterns: chicken_*) ─────────────────────────────
class ChickenManager {
public:
    static ChickenManager& Instance() { static ChickenManager m; return m; }

    // Pattern targets: chicken_update_model, chicken_apply_look,
    // chicken_set_activity

    void UpdateChickenModel(void* chicken, int model_index) {
        // chicken_update_model
    }

    void ApplyLook(void* chicken, const SkinConfig& skin) {
        // chicken_apply_look
    }

    void SetActivity(void* chicken, int activity) {
        // chicken_set_activity
    }
};

} // namespace inventory
