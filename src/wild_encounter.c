#include "global.h"
#include "random.h"
#include "wild_encounter.h"
#include "event_data.h"
#include "fieldmap.h"
#include "random.h"
#include "roamer.h"
#include "field_player_avatar.h"
#include "battle_setup.h"
#include "overworld.h"
#include "metatile_behavior.h"
#include "event_scripts.h"
#include "script.h"
#include "link.h"
#include "quest_log.h"
#include "constants/maps.h"
#include "constants/abilities.h"
#include "constants/items.h"

#define MAX_RSE_ENCOUNTER_RATE 2880
#define MAX_FRLG_ENCOUNTER_RATE 1600

#define HEADER_NONE 0xFFFF

enum
{
    WILD_AREA_LAND,
    WILD_AREA_WATER,
    WILD_AREA_ROCKS,
    WILD_AREA_FISHING,
};

#define WILD_CHECK_REPEL    0x1
#define WILD_CHECK_KEEN_EYE 0x2

struct WildEncounterData
{
    u32 rngState;
    u16 prevMetatileBehavior;
    u16 encounterRateBuff;
    u8 stepsSinceLastEncounter;
    u8 abilityEffect;
    u16 leadMonHeldItem;
};

static EWRAM_DATA struct WildEncounterData sWildEncounterData = {};
static EWRAM_DATA bool8 sWildEncountersDisabled = FALSE;
EWRAM_DATA u32 gEncounterMode = ENCOUNTER_FIRERED;
EWRAM_DATA static u32 sEncounterRateBuff = 0;
EWRAM_DATA static u32 sMaxLevel = 0;
EWRAM_DATA static struct TempMon sTempMons[3] = {0};
EWRAM_DATA static u32 sSavedRateCheck = 0;

static bool8 UnlockedTanobyOrAreNotInTanoby(void);
static u32 GenerateUnownPersonalityByLetter(u8 letter);
static bool32 IsWildLevelAllowedByRepel(u32 level);
static void ApplyFluteEncounterRateMod(u32 *rate);
static u8 GetFluteEncounterRateModType(void);
static void ApplyCleanseTagEncounterRateMod(u32 *rate);
static bool8 IsLeadMonHoldingCleanseTag(void);
static void AddToWildEncounterRateBuff(u8 encouterRate);
static inline u32 GenerateEncounter(u32 headerId, u32 curMetatileBehavior, u32 prevMetatileBehavior, const struct WildPokemonInfo *wildPokemonInfo, u32 terrain, bool32 bypassCheck, u32 partySlot);

#include "data/wild_encounters_frlg.h"

static const u8 sUnownLetterSlots[][LAND_WILD_COUNT] = {
  //  A   A   A   A   A   A   A   A   A   A   A   ?
    { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 27},
  //  C   C   C   D   D   D   H   H   H   U   U   O
    { 2,  2,  2,  3,  3,  3,  7,  7,  7, 20, 20, 14},
  //  N   N   N   N   S   S   S   S   I   I   E   E
    {13, 13, 13, 13, 18, 18, 18, 18,  8,  8,  4,  4},
  //  P   P   L   L   J   J   R   R   R   Q   Q   Q
    {15, 15, 11, 11,  9,  9, 17, 17, 17, 16, 16, 16},
  //  Y   Y   T   T   G   G   G   F   F   F   K   K
    {24, 24, 19, 19,  6,  6,  6,  5,  5,  5, 10, 10},
  //  V   V   V   W   W   W   X   X   M   M   B   B
    {21, 21, 21, 22, 22, 22, 23, 23, 12, 12,  1,  1},
  //  Z   Z   Z   Z   Z   Z   Z   Z   Z   Z   Z   !
    {25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 26},
};

void DisableWildEncounters(bool8 state)
{
    sWildEncountersDisabled = state;
}

static u8 ChooseWildMonIndex_Land(void)
{
    u8 rand = Random() % ENCOUNTER_CHANCE_LAND_MONS_TOTAL;

    if (rand < ENCOUNTER_CHANCE_LAND_MONS_SLOT_0)
        return 0;
    else if (rand >= ENCOUNTER_CHANCE_LAND_MONS_SLOT_0 && rand < ENCOUNTER_CHANCE_LAND_MONS_SLOT_1)
        return 1;
    else if (rand >= ENCOUNTER_CHANCE_LAND_MONS_SLOT_1 && rand < ENCOUNTER_CHANCE_LAND_MONS_SLOT_2)
        return 2;
    else if (rand >= ENCOUNTER_CHANCE_LAND_MONS_SLOT_2 && rand < ENCOUNTER_CHANCE_LAND_MONS_SLOT_3)
        return 3;
    else if (rand >= ENCOUNTER_CHANCE_LAND_MONS_SLOT_3 && rand < ENCOUNTER_CHANCE_LAND_MONS_SLOT_4)
        return 4;
    else if (rand >= ENCOUNTER_CHANCE_LAND_MONS_SLOT_4 && rand < ENCOUNTER_CHANCE_LAND_MONS_SLOT_5)
        return 5;
    else if (rand >= ENCOUNTER_CHANCE_LAND_MONS_SLOT_5 && rand < ENCOUNTER_CHANCE_LAND_MONS_SLOT_6)
        return 6;
    else if (rand >= ENCOUNTER_CHANCE_LAND_MONS_SLOT_6 && rand < ENCOUNTER_CHANCE_LAND_MONS_SLOT_7)
        return 7;
    else if (rand >= ENCOUNTER_CHANCE_LAND_MONS_SLOT_7 && rand < ENCOUNTER_CHANCE_LAND_MONS_SLOT_8)
        return 8;
    else if (rand >= ENCOUNTER_CHANCE_LAND_MONS_SLOT_8 && rand < ENCOUNTER_CHANCE_LAND_MONS_SLOT_9)
        return 9;
    else if (rand >= ENCOUNTER_CHANCE_LAND_MONS_SLOT_9 && rand < ENCOUNTER_CHANCE_LAND_MONS_SLOT_10)
        return 10;
    else
        return 11;
}

static u8 ChooseWildMonIndex_WaterRock(void)
{
    u8 rand = Random() % ENCOUNTER_CHANCE_WATER_MONS_TOTAL;

    if (rand < ENCOUNTER_CHANCE_WATER_MONS_SLOT_0)
        return 0;
    else if (rand >= ENCOUNTER_CHANCE_WATER_MONS_SLOT_0 && rand < ENCOUNTER_CHANCE_WATER_MONS_SLOT_1)
        return 1;
    else if (rand >= ENCOUNTER_CHANCE_WATER_MONS_SLOT_1 && rand < ENCOUNTER_CHANCE_WATER_MONS_SLOT_2)
        return 2;
    else if (rand >= ENCOUNTER_CHANCE_WATER_MONS_SLOT_2 && rand < ENCOUNTER_CHANCE_WATER_MONS_SLOT_3)
        return 3;
    else
        return 4;
}

static u8 ChooseWildMonIndex_Fishing(u8 rod)
{
    u8 wildMonIndex = 0;
    u8 rand = Random() % max(max(ENCOUNTER_CHANCE_FISHING_MONS_OLD_ROD_TOTAL, ENCOUNTER_CHANCE_FISHING_MONS_GOOD_ROD_TOTAL),
                             ENCOUNTER_CHANCE_FISHING_MONS_SUPER_ROD_TOTAL);

    switch (rod)
    {
    case OLD_ROD:
        if (rand < ENCOUNTER_CHANCE_FISHING_MONS_OLD_ROD_SLOT_0)
            wildMonIndex = 0;
        else
            wildMonIndex = 1;
        break;
    case GOOD_ROD:
        if (rand < ENCOUNTER_CHANCE_FISHING_MONS_GOOD_ROD_SLOT_2)
            wildMonIndex = 2;
        if (rand >= ENCOUNTER_CHANCE_FISHING_MONS_GOOD_ROD_SLOT_2 && rand < ENCOUNTER_CHANCE_FISHING_MONS_GOOD_ROD_SLOT_3)
            wildMonIndex = 3;
        if (rand >= ENCOUNTER_CHANCE_FISHING_MONS_GOOD_ROD_SLOT_3 && rand < ENCOUNTER_CHANCE_FISHING_MONS_GOOD_ROD_SLOT_4)
            wildMonIndex = 4;
        break;
    case SUPER_ROD:
        if (rand < ENCOUNTER_CHANCE_FISHING_MONS_SUPER_ROD_SLOT_5)
            wildMonIndex = 5;
        if (rand >= ENCOUNTER_CHANCE_FISHING_MONS_SUPER_ROD_SLOT_5 && rand < ENCOUNTER_CHANCE_FISHING_MONS_SUPER_ROD_SLOT_6)
            wildMonIndex = 6;
        if (rand >= ENCOUNTER_CHANCE_FISHING_MONS_SUPER_ROD_SLOT_6 && rand < ENCOUNTER_CHANCE_FISHING_MONS_SUPER_ROD_SLOT_7)
            wildMonIndex = 7;
        if (rand >= ENCOUNTER_CHANCE_FISHING_MONS_SUPER_ROD_SLOT_7 && rand < ENCOUNTER_CHANCE_FISHING_MONS_SUPER_ROD_SLOT_8)
            wildMonIndex = 8;
        if (rand >= ENCOUNTER_CHANCE_FISHING_MONS_SUPER_ROD_SLOT_8 && rand < ENCOUNTER_CHANCE_FISHING_MONS_SUPER_ROD_SLOT_9)
            wildMonIndex = 9;
        break;
    }
    return wildMonIndex;
}

static u8 ChooseWildMonLevel(const struct WildPokemon * info)
{
    u8 lo;
    u8 hi;
    u8 mod;
    u8 res;
    if (info->maxLevel >= info->minLevel)
    {
        lo = info->minLevel;
        hi = info->maxLevel;
    }
    else
    {
        lo = info->maxLevel;
        hi = info->minLevel;
    }
    mod = hi - lo + 1;
    res = Random() % mod;
    return lo + res;
}

static inline u32 GetFireRedWildMonHeaderId(void)
{
    u32 i;

    for (i = 0; ; i++)
    {
        const struct WildPokemonHeader *wildHeader = &gWildMonHeadersFRLG[i];

        if (wildHeader->mapGroup == MAP_GROUP(MAP_UNDEFINED))
            break;
    
        if (gWildMonHeadersFRLG[i].mapGroup == gSaveBlock1Ptr->location.mapGroup &&
            gWildMonHeadersFRLG[i].mapNum == gSaveBlock1Ptr->location.mapNum)
        {
            if (gSaveBlock1Ptr->location.mapGroup == MAP_GROUP(MAP_SIX_ISLAND_ALTERING_CAVE) &&
                gSaveBlock1Ptr->location.mapNum == MAP_NUM(MAP_SIX_ISLAND_ALTERING_CAVE))
            {
                u32 alteringCaveId = VarGet(VAR_ALTERING_CAVE_WILD_SET);

                if (alteringCaveId >= NUM_ALTERING_CAVE_TABLES)
                    alteringCaveId = 0;
    
                i += alteringCaveId;
            }

            if (!UnlockedTanobyOrAreNotInTanoby())
                break;

            return i;
        }
    }
    
    return HEADER_NONE;
}

static inline u32 GetCorrespondingLeafGreenWildMonHeaderId(u16 currentId)
{
    u32 i;

    for (i = currentId + 1; ; i++)
    {
        const struct WildPokemonHeader *wildHeader = &gWildMonHeadersFRLG[i];

        if (wildHeader->mapGroup == MAP_GROUP(MAP_UNDEFINED))
            break;
    
        if (gWildMonHeadersFRLG[i].mapGroup == gSaveBlock1Ptr->location.mapGroup &&
            gWildMonHeadersFRLG[i].mapNum == gSaveBlock1Ptr->location.mapNum)
        {
            if (gSaveBlock1Ptr->location.mapGroup == MAP_GROUP(MAP_SIX_ISLAND_ALTERING_CAVE) &&
                gSaveBlock1Ptr->location.mapNum == MAP_NUM(MAP_SIX_ISLAND_ALTERING_CAVE))
            {
                u32 alteringCaveId = VarGet(VAR_ALTERING_CAVE_WILD_SET);

                if (alteringCaveId >= NUM_ALTERING_CAVE_TABLES)
                    alteringCaveId = 0;
    
                i += alteringCaveId;
            }
    

            if (!UnlockedTanobyOrAreNotInTanoby())
                break;

            return i;
        }
    }
    
    return HEADER_NONE;
}

static bool8 UnlockedTanobyOrAreNotInTanoby(void)
{
    if (FlagGet(FLAG_SYS_UNLOCKED_TANOBY_RUINS))
        return TRUE;
    if (gSaveBlock1Ptr->location.mapGroup != MAP_GROUP(MAP_SEVEN_ISLAND_TANOBY_RUINS_DILFORD_CHAMBER))
        return TRUE;
    if (!(gSaveBlock1Ptr->location.mapNum == MAP_NUM(MAP_SEVEN_ISLAND_TANOBY_RUINS_MONEAN_CHAMBER)
    ||  gSaveBlock1Ptr->location.mapNum == MAP_NUM(MAP_SEVEN_ISLAND_TANOBY_RUINS_LIPTOO_CHAMBER)
    ||  gSaveBlock1Ptr->location.mapNum == MAP_NUM(MAP_SEVEN_ISLAND_TANOBY_RUINS_WEEPTH_CHAMBER)
    ||  gSaveBlock1Ptr->location.mapNum == MAP_NUM(MAP_SEVEN_ISLAND_TANOBY_RUINS_DILFORD_CHAMBER)
    ||  gSaveBlock1Ptr->location.mapNum == MAP_NUM(MAP_SEVEN_ISLAND_TANOBY_RUINS_SCUFIB_CHAMBER)
    ||  gSaveBlock1Ptr->location.mapNum == MAP_NUM(MAP_SEVEN_ISLAND_TANOBY_RUINS_RIXY_CHAMBER)
    ||  gSaveBlock1Ptr->location.mapNum == MAP_NUM(MAP_SEVEN_ISLAND_TANOBY_RUINS_VIAPOIS_CHAMBER)
    ))
        return TRUE;
    return FALSE;
}

u8 GetUnownLetterByPersonalityLoByte(u32 personality)
{
    return GET_UNOWN_LETTER(personality);
}

static inline void CreateWildMon(u32 species, u32 level, u32 partySlot)
{
    struct PIDParameters parameters;

    parameters.species = species;
    parameters.pidIVMethod = PIDIV_METHOD_1;
    parameters.shinyLock = GENERATE_SHINY_NORMAL;
    parameters.shinyRolls = 1;
    parameters.forceNature = TRUE;
    parameters.nature = Random() & NUM_NATURES;
    parameters.forceGender = FALSE;
    parameters.gender = 0;
    parameters.forceUnownLetter = FALSE;
    parameters.unownLetter = 0;

    sTempMons[partySlot + 1].pid = GeneratePIDMaster(parameters, &(sTempMons[partySlot + 1].ivs));
    sTempMons[partySlot + 1].species = species;
    sTempMons[partySlot + 1].level = level;
}

static inline void CreateWildUnown(u32 slot, u32 level, u32 partySlot)
{
    struct PIDParameters parameters;
    struct IVs ivs;

    parameters.species = SPECIES_UNOWN;
    parameters.pidIVMethod = PIDIV_METHOD_REVERSE_U;
    parameters.shinyLock = GENERATE_SHINY_NORMAL;
    parameters.shinyRolls = 1;
    parameters.forceNature = FALSE;
    parameters.nature = 0;
    parameters.forceGender = FALSE;
    parameters.gender = 0;
    parameters.forceUnownLetter = TRUE;
    parameters.unownLetter = sUnownLetterSlots[(gSaveBlock1Ptr->location.mapNum - MAP_NUM(MAP_SEVEN_ISLAND_TANOBY_RUINS_MONEAN_CHAMBER))][slot];


    sTempMons[partySlot + 1].pid = GeneratePIDMaster(parameters, &(sTempMons[partySlot + 1].ivs));
    sTempMons[partySlot + 1].species = SPECIES_UNOWN;
    sTempMons[partySlot + 1].level = level;
}

static inline bool32 TryGenerateWildMon(const struct WildPokemonInfo *wildMonInfo, u32 area, u32 flags, u32 partySlot)
{
    u32 wildMonIndex = 0;
    u32 level;

    switch (area)
    {
    case WILD_AREA_LAND:
        wildMonIndex = ChooseWildMonIndex_Land();
        break;
    case WILD_AREA_WATER:
        wildMonIndex = ChooseWildMonIndex_WaterRock();
        break;
    case WILD_AREA_ROCKS:
        wildMonIndex = ChooseWildMonIndex_WaterRock();
        break;
    }

    level = ChooseWildMonLevel(&wildMonInfo->wildPokemon[wildMonIndex]);

    if (flags & WILD_CHECK_REPEL && !IsWildLevelAllowedByRepel(level))
        return FALSE;

    // if (gEncounterMode == ENCOUNTER_HELIODOR)
    // {
    //     gEncounterMode = wildMonInfo->wildPokemon[wildMonIndex].originGame;
// 
    //     if (gEncounterMode == ENCOUNTER_RS)
    //     {
    //         if (Random2() % 2 == 0)
    //             gEncounterMode = ENCOUNTER_RUBY;
    //         else
    //             gEncounterMode = ENCOUNTER_SAPPHIRE;
    //     }
    //     else if (gEncounterMode == ENCOUNTER_FRLG)
    //     {
    //         if (Random2() % 2 == 0)
    //             gEncounterMode = ENCOUNTER_FIRERED;
    //         else
    //             gEncounterMode = ENCOUNTER_LEAFGREEN;
    //     }
// 
    //     if (wildMonInfo->wildPokemon[wildMonIndex].encounterRate != 0 && WildEncounterCheck(wildMonInfo->wildPokemon[wildMonIndex].encounterRate, FALSE, TRUE) == FALSE)
    //         return FALSE;
    // }

    if (wildMonInfo->wildPokemon[wildMonIndex].species == SPECIES_UNOWN)
        CreateWildUnown(wildMonIndex, level, partySlot);
    else
        CreateWildMon(wildMonInfo->wildPokemon[wildMonIndex].species, level, partySlot);

    return TRUE;
}

static inline u32 GenerateFishingWildMon(const struct WildPokemonInfo *wildMonInfo, u32 rod)
{
    u32 wildMonIndex;
    u32 level;

    wildMonIndex = ChooseWildMonIndex_Fishing(rod);
    level = ChooseWildMonLevel(&wildMonInfo->wildPokemon[wildMonIndex]);

    CreateWildMon(wildMonInfo->wildPokemon[wildMonIndex].species, level, 0);

    return wildMonInfo->wildPokemon[wildMonIndex].species;
}

static inline bool32 EncounterOddsCheck(u32 encounterRate, u32 maxRate, bool32 recheck)
{
    if (!recheck)
        sSavedRateCheck = Random();

    if (sSavedRateCheck % maxRate < encounterRate)
        return TRUE;
    else
        return FALSE;
}

// Returns true if it will try to create a wild encounter.
static inline bool32 WildEncounterCheck(u32 encounterRate, bool32 ignoreAbility, bool32 recheck)
{
    switch (gEncounterMode)
    {
        case ENCOUNTER_RUBY:
        case ENCOUNTER_SAPPHIRE:
            encounterRate *= 16;

            if (TestPlayerAvatarFlags(PLAYER_AVATAR_FLAG_MACH_BIKE | PLAYER_AVATAR_FLAG_ACRO_BIKE))
                encounterRate = encounterRate * 80 / 100;

            ApplyFluteEncounterRateMod(&encounterRate);
            ApplyCleanseTagEncounterRateMod(&encounterRate);

            if (!ignoreAbility && sTempMons[0].notEgg)
            {
                if (sTempMons[0].ability == ABILITY_STENCH)
                    encounterRate /= 2;
                else if (sTempMons[0].ability == ABILITY_ILLUMINATE)
                    encounterRate *= 2;
            }

            if (encounterRate > MAX_RSE_ENCOUNTER_RATE)
                encounterRate = MAX_RSE_ENCOUNTER_RATE;

            return EncounterOddsCheck(encounterRate, MAX_RSE_ENCOUNTER_RATE, recheck);
        case ENCOUNTER_FIRERED:
        case ENCOUNTER_LEAFGREEN:
            encounterRate *= 16;

            if (TestPlayerAvatarFlags(PLAYER_AVATAR_FLAG_MACH_BIKE | PLAYER_AVATAR_FLAG_ACRO_BIKE))
                encounterRate = encounterRate * 80 / 100;

            encounterRate += sEncounterRateBuff * 16 / 200;
            ApplyFluteEncounterRateMod(&encounterRate);
            ApplyCleanseTagEncounterRateMod(&encounterRate);

            if (!ignoreAbility && sTempMons[0].notEgg)
            {
                if (sTempMons[0].ability == ABILITY_STENCH)
                    encounterRate /= 2;
                else if (sTempMons[0].ability == ABILITY_ILLUMINATE)
                    encounterRate *= 2;
            }

            if (encounterRate > MAX_FRLG_ENCOUNTER_RATE)
                encounterRate = MAX_FRLG_ENCOUNTER_RATE;

            return EncounterOddsCheck(encounterRate, MAX_FRLG_ENCOUNTER_RATE, recheck);
        case ENCOUNTER_EMERALD:
        default:
            encounterRate *= 16;

            if (TestPlayerAvatarFlags(PLAYER_AVATAR_FLAG_MACH_BIKE | PLAYER_AVATAR_FLAG_ACRO_BIKE))
                encounterRate = encounterRate * 80 / 100;

            ApplyFluteEncounterRateMod(&encounterRate);
            ApplyCleanseTagEncounterRateMod(&encounterRate);

            if (!ignoreAbility && sTempMons[0].notEgg)
            {
                if (sTempMons[0].ability == ABILITY_STENCH)
                    encounterRate /= 2;
                else if (sTempMons[0].ability == ABILITY_ILLUMINATE)
                    encounterRate *= 2;
                //else if (sTempMons[0].ability == ABILITY_WHITE_SMOKE)
                //    encounterRate /= 2;
                //else if (sTempMons[0].ability == ABILITY_ARENA_TRAP)
                //    encounterRate *= 2;
                //else if (sTempMons[0].ability == ABILITY_SAND_VEIL && gSaveBlock1Ptr->weather == WEATHER_SANDSTORM)
                //    encounterRate /= 2;
            }

            if (encounterRate > MAX_RSE_ENCOUNTER_RATE)
                encounterRate = MAX_RSE_ENCOUNTER_RATE;

            return EncounterOddsCheck(encounterRate, MAX_RSE_ENCOUNTER_RATE, recheck);
    }
}

// When you first step on a different type of metatile, there's a 40% chance it
// skips the wild encounter check entirely.
static inline bool32 AllowWildCheckOnNewMetatile(void)
{
    if (Random() % 100 >= 60)
        return FALSE;
    else
        return TRUE;
}

bool32 StandardWildEncounter(u32 currMetatileAttrs, u32 previousMetatileBehavior)
{
    u32 headerId, oppositeHeaderId, encounterResult;
    struct Roamer * roamer;
    const struct WildPokemonInfo *wildPokemonInfo;

    if (sWildEncountersDisabled == TRUE)
        return FALSE;

    headerId = GetFireRedWildMonHeaderId();
    gEncounterMode = ENCOUNTER_FIRERED;
    ZeroEnemyPartyMons();

    if (headerId != HEADER_NONE)
    {
        if (ExtractMetatileAttribute(currMetatileAttrs, METATILE_ATTRIBUTE_ENCOUNTER_TYPE) == TILE_ENCOUNTER_LAND)
        {
            if (previousMetatileBehavior != ExtractMetatileAttribute(currMetatileAttrs, METATILE_ATTRIBUTE_BEHAVIOR))
                return FALSE;

            if (gEncounterMode == ENCOUNTER_FIRERED)
            {
                wildPokemonInfo = gWildMonHeadersFRLG[headerId].landMonsInfo;

                switch (Random() % 2)
                {
                    case 1:
                        oppositeHeaderId = GetCorrespondingLeafGreenWildMonHeaderId(headerId);

                        if (oppositeHeaderId != HEADER_NONE)
                        {
                            gEncounterMode = ENCOUNTER_LEAFGREEN;
                            wildPokemonInfo = gWildMonHeadersFRLG[oppositeHeaderId].landMonsInfo;
                        }
                        break;
                    //case 2:
                    //    headerId = GetHeliodorWildMonHeaderId();
//
                    //    if (headerId != HEADER_NONE)
                    //    {
                    //        gEncounterMode = ENCOUNTER_HELIODOR;
                    //        wildPokemonInfo = gWildMonHeaders[headerId].landMonsInfo;
                    //    }
                    //    break;
                }
            }


            if (wildPokemonInfo == NULL)
                return FALSE;

            encounterResult = GenerateEncounter(headerId, ExtractMetatileAttribute(currMetatileAttrs, METATILE_ATTRIBUTE_BEHAVIOR), previousMetatileBehavior, wildPokemonInfo, WILD_AREA_LAND, FALSE, 0);

            if (encounterResult == ENCOUNTER_ROAMER_1)
            {
                roamer = &gSaveBlock1Ptr->roamer;

                if (!IsWildLevelAllowedByRepel(roamer->level))
                    return FALSE;
            
                StartRoamerBattle();
                return TRUE;
            }
            else if (encounterResult == ENCOUNTER_ROAMER_2)
            {
                roamer = &gSaveBlock1Ptr->extraRoamers[0];

                if (!IsWildLevelAllowedByRepel(roamer->level))
                    return FALSE;
            
                StartRoamerBattle();
                return TRUE;
            }
            else if (encounterResult == ENCOUNTER_ROAMER_3)
            {
                roamer = &gSaveBlock1Ptr->extraRoamers[1];

                if (!IsWildLevelAllowedByRepel(roamer->level))
                    return FALSE;
            
                StartRoamerBattle();
                return TRUE;
            }
            else if (encounterResult == ENCOUNTER_SUCCESS)
            {
                if (IsMonShiny(&gEnemyParty[0]))
                    IncrementGameStat(GAME_STAT_SHINIES_FOUND);
 
                StartWildBattle();
                return TRUE;
            }
            else
            {
                return FALSE;
            }
        }
        else if (ExtractMetatileAttribute(currMetatileAttrs, METATILE_ATTRIBUTE_ENCOUNTER_TYPE) == TILE_ENCOUNTER_WATER
                 || (TestPlayerAvatarFlags(PLAYER_AVATAR_FLAG_SURFING) && MetatileBehavior_IsBridge(ExtractMetatileAttribute(currMetatileAttrs, METATILE_ATTRIBUTE_BEHAVIOR)) == TRUE))
        {
            if (previousMetatileBehavior != ExtractMetatileAttribute(currMetatileAttrs, METATILE_ATTRIBUTE_BEHAVIOR))
                return FALSE;

            if (gEncounterMode == ENCOUNTER_FIRERED)
            {
                wildPokemonInfo = gWildMonHeadersFRLG[headerId].waterMonsInfo;

                switch (Random() % 2)
                {
                    case 1:
                        oppositeHeaderId = GetCorrespondingLeafGreenWildMonHeaderId(headerId);

                        if (oppositeHeaderId != HEADER_NONE)
                        {
                            gEncounterMode = ENCOUNTER_LEAFGREEN;
                            wildPokemonInfo = gWildMonHeadersFRLG[oppositeHeaderId].waterMonsInfo;
                        }
                        break;
                    //case 2:
                    //    headerId = GetHeliodorWildMonHeaderId();
//
                    //    if (headerId != HEADER_NONE)
                    //    {
                    //        gEncounterMode = ENCOUNTER_HELIODOR;
                    //        wildPokemonInfo = gWildMonHeaders[headerId].landMonsInfo;
                    //    }
                    //    break;
                }
            }


            if (wildPokemonInfo == NULL)
                return FALSE;

            encounterResult = GenerateEncounter(headerId, ExtractMetatileAttribute(currMetatileAttrs, METATILE_ATTRIBUTE_BEHAVIOR), previousMetatileBehavior, wildPokemonInfo, WILD_AREA_WATER, FALSE, 0);

            if (encounterResult == ENCOUNTER_ROAMER_1)
            {
                roamer = &gSaveBlock1Ptr->roamer;

                if (!IsWildLevelAllowedByRepel(roamer->level))
                    return FALSE;
            
                StartRoamerBattle();
                return TRUE;
            }
            else if (encounterResult == ENCOUNTER_ROAMER_2)
            {
                roamer = &gSaveBlock1Ptr->extraRoamers[0];

                if (!IsWildLevelAllowedByRepel(roamer->level))
                    return FALSE;
            
                StartRoamerBattle();
                return TRUE;
            }
            else if (encounterResult == ENCOUNTER_ROAMER_3)
            {
                roamer = &gSaveBlock1Ptr->extraRoamers[1];

                if (!IsWildLevelAllowedByRepel(roamer->level))
                    return FALSE;
            
                StartRoamerBattle();
                return TRUE;
            }
            else if (encounterResult == ENCOUNTER_SUCCESS)
            {
                if (IsMonShiny(&gEnemyParty[0]))
                    IncrementGameStat(GAME_STAT_SHINIES_FOUND);
 
                StartWildBattle();
                return TRUE;
            }
            else
            {
                return FALSE;
            }
        }
    }

    return FALSE;
}

void RockSmashWildEncounter(void)
{
    u32 flags = WILD_CHECK_REPEL;
    u32 oppositeHeaderId;
    u32 headerId = GetFireRedWildMonHeaderId();

    gEncounterMode = ENCOUNTER_FIRERED;

    if (headerId != HEADER_NONE)
    {
        const struct WildPokemonInfo *wildPokemonInfo;

        if (gEncounterMode == ENCOUNTER_FIRERED)
        {
            wildPokemonInfo = gWildMonHeadersFRLG[headerId].rockSmashMonsInfo;

            switch (Random() % 2)
            {
                case 1:
                    oppositeHeaderId = GetCorrespondingLeafGreenWildMonHeaderId(headerId);

                    if (oppositeHeaderId != HEADER_NONE)
                    {
                        gEncounterMode = ENCOUNTER_LEAFGREEN;
                        wildPokemonInfo = gWildMonHeadersFRLG[oppositeHeaderId].rockSmashMonsInfo;
                    }
                    break;
                //case 2:
                //    headerId = GetHeliodorWildMonHeaderId();
//
                //    if (headerId != HEADER_NONE)
                //    {
                //        gEncounterMode = ENCOUNTER_HELIODOR;
                //        wildPokemonInfo = gWildMonHeaders[headerId].rockSmashMonsInfo;
                //    }
                //    break;
            }
        }

        if (wildPokemonInfo == NULL)
        {
            gSpecialVar_Result = FALSE;
        }
        else if (WildEncounterCheck(wildPokemonInfo->encounterRate, TRUE, FALSE) == TRUE
         && TryGenerateWildMon(wildPokemonInfo, WILD_AREA_ROCKS, flags, 0) == TRUE)
        {
            if (IsMonShiny(&gEnemyParty[0]))
                IncrementGameStat(GAME_STAT_SHINIES_FOUND);

            StartWildBattle();
            gSpecialVar_Result = TRUE;
        }
        else
        {
            gSpecialVar_Result = FALSE;
        }
    }
    else
    {
        gSpecialVar_Result = FALSE;
    }
}

bool32 SweetScentWildEncounter(void)
{
    struct Roamer *roamer;
    const struct WildPokemonInfo *wildPokemonInfo;
    s16 x, y;
    u32 encounterResult;
    u32 oppositeHeaderId;
    u32 headerId = GetFireRedWildMonHeaderId();

    gEncounterMode = ENCOUNTER_FIRERED;

    PlayerGetDestCoords(&x, &y);
    if (headerId != HEADER_NONE)
    {
        if (MapGridGetMetatileAttributeAt(x, y, METATILE_ATTRIBUTE_ENCOUNTER_TYPE) == TILE_ENCOUNTER_LAND)
        {
            if (gEncounterMode == ENCOUNTER_FIRERED)
            {
                wildPokemonInfo = gWildMonHeadersFRLG[headerId].landMonsInfo;
    
                switch (Random() % 2)
                {
                    case 1:
                        oppositeHeaderId = GetCorrespondingLeafGreenWildMonHeaderId(headerId);
    
                        if (oppositeHeaderId != HEADER_NONE)
                        {
                            gEncounterMode = ENCOUNTER_LEAFGREEN;
                            wildPokemonInfo = gWildMonHeadersFRLG[oppositeHeaderId].landMonsInfo;
                        }
                        break;
                    //case 2:
                    //    headerId = GetHeliodorWildMonHeaderId();
//
                    //    if (headerId != HEADER_NONE)
                    //    {
                    //        gEncounterMode = ENCOUNTER_HELIODOR;
                    //        wildPokemonInfo = gWildMonHeaders[headerId].landMonsInfo;
                    //    }
                    //    break;
                }
            }
    
            if (wildPokemonInfo == NULL)
                return FALSE;
    
            encounterResult = GenerateEncounter(headerId, 0, 0, wildPokemonInfo, WILD_AREA_LAND, TRUE, 0);

            if (encounterResult == ENCOUNTER_ROAMER_1)
            {
                roamer = &gSaveBlock1Ptr->roamer;

                if (!IsWildLevelAllowedByRepel(roamer->level))
                    return FALSE;
            
                StartRoamerBattle();
                return TRUE;
            }
            else if (encounterResult == ENCOUNTER_ROAMER_2)
            {
                roamer = &gSaveBlock1Ptr->extraRoamers[0];

                if (!IsWildLevelAllowedByRepel(roamer->level))
                    return FALSE;
            
                StartRoamerBattle();
                return TRUE;
            }
            else if (encounterResult == ENCOUNTER_ROAMER_3)
            {
                roamer = &gSaveBlock1Ptr->extraRoamers[1];

                if (!IsWildLevelAllowedByRepel(roamer->level))
                    return FALSE;
            
                StartRoamerBattle();
                return TRUE;
            }
            else if (encounterResult == ENCOUNTER_SUCCESS)
            {
                if (IsMonShiny(&gEnemyParty[0]))
                     IncrementGameStat(GAME_STAT_SHINIES_FOUND);
 
                StartWildBattle();
                return TRUE;
            }
            else
            {
                return FALSE;
            }
        }
        else if (MapGridGetMetatileAttributeAt(x, y, METATILE_ATTRIBUTE_ENCOUNTER_TYPE) == TILE_ENCOUNTER_WATER)
        {
            if (gEncounterMode == ENCOUNTER_FIRERED)
            {
                wildPokemonInfo = gWildMonHeadersFRLG[headerId].waterMonsInfo;
    
                switch (Random() % 2)
                {
                    case 1:
                        oppositeHeaderId = GetCorrespondingLeafGreenWildMonHeaderId(headerId);
    
                        if (oppositeHeaderId != HEADER_NONE)
                        {
                            gEncounterMode = ENCOUNTER_LEAFGREEN;
                            wildPokemonInfo = gWildMonHeadersFRLG[oppositeHeaderId].waterMonsInfo;
                        }
                        break;
                    //case 2:
                    //    headerId = GetHeliodorWildMonHeaderId();
//
                    //    if (headerId != HEADER_NONE)
                    //    {
                    //        gEncounterMode = ENCOUNTER_HELIODOR;
                    //        wildPokemonInfo = gWildMonHeaders[headerId].landMonsInfo;
                    //    }
                    //    break;
                }
            }
    
            if (wildPokemonInfo == NULL)
                return FALSE;
    
            encounterResult = GenerateEncounter(headerId, 0, 0, wildPokemonInfo, WILD_AREA_WATER, TRUE, 0);

            if (encounterResult == ENCOUNTER_ROAMER_1)
            {
                roamer = &gSaveBlock1Ptr->roamer;

                if (!IsWildLevelAllowedByRepel(roamer->level))
                    return FALSE;
            
                StartRoamerBattle();
                return TRUE;
            }
            else if (encounterResult == ENCOUNTER_ROAMER_2)
            {
                roamer = &gSaveBlock1Ptr->extraRoamers[0];

                if (!IsWildLevelAllowedByRepel(roamer->level))
                    return FALSE;
            
                StartRoamerBattle();
                return TRUE;
            }
            else if (encounterResult == ENCOUNTER_ROAMER_3)
            {
                roamer = &gSaveBlock1Ptr->extraRoamers[1];

                if (!IsWildLevelAllowedByRepel(roamer->level))
                    return FALSE;
            
                StartRoamerBattle();
                return TRUE;
            }
            else if (encounterResult == ENCOUNTER_SUCCESS)
            {
                if (IsMonShiny(&gEnemyParty[0]))
                     IncrementGameStat(GAME_STAT_SHINIES_FOUND);
 
                StartWildBattle();
                return TRUE;
            }
            else
            {
                return FALSE;
            }
        }
    }

    return FALSE;
}

bool32 DoesCurrentMapHaveFishingMons(void)
{
    u32 rubyHeaderId, sapphireHeaderId, leafgreenHeaderId, heliodorHeaderId;
    u32 headerId = GetFireRedWildMonHeaderId();

    if (headerId == HEADER_NONE)
    {
        return FALSE;
    }
    else
    {
        leafgreenHeaderId = GetCorrespondingLeafGreenWildMonHeaderId(headerId);

        if (headerId != HEADER_NONE && (gWildMonHeadersFRLG[headerId].fishingMonsInfo != NULL || gWildMonHeadersFRLG[leafgreenHeaderId].fishingMonsInfo != NULL))
            return TRUE;
    }

    return FALSE;
}

void FishingWildEncounter(u32 rod)
{
    const struct WildPokemonInfo *wildPokemonInfo;
    u32 species, oppositeHeaderId;
    u32 headerId = GetFireRedWildMonHeaderId();

    gEncounterMode = ENCOUNTER_FIRERED;

    if (headerId != HEADER_NONE)
    {
        wildPokemonInfo = gWildMonHeadersFRLG[headerId].fishingMonsInfo;

        switch (Random() % 2)
        {
            case 1:
                oppositeHeaderId = GetCorrespondingLeafGreenWildMonHeaderId(headerId);
                if (oppositeHeaderId != HEADER_NONE)
                {
                    gEncounterMode = ENCOUNTER_LEAFGREEN;
                    wildPokemonInfo = gWildMonHeadersFRLG[oppositeHeaderId].fishingMonsInfo;
                }
                break;
            //case 2:
            //    headerId = GetHeliodorWildMonHeaderId();
            //    if (headerId != HEADER_NONE)
            //    {
            //        gEncounterMode = ENCOUNTER_HELIODOR;
            //        wildPokemonInfo = gWildMonHeaders[headerId].fishingMonsInfo;
            //    }
            //    break;
        }

        species = GenerateFishingWildMon(wildPokemonInfo, rod);
    }

    IncrementGameStat(GAME_STAT_FISHING_CAPTURES);

    if (IsMonShiny(&gEnemyParty[0]))
        IncrementGameStat(GAME_STAT_SHINIES_FOUND);

    StartWildBattle();
}

u32 GetLocalWildMon(bool8 *isWaterMon)
{
    const struct WildPokemonInfo *landMonsInfo;
    const struct WildPokemonInfo *waterMonsInfo;
    u32 oppositeHeaderId;
    u32 headerId = GetFireRedWildMonHeaderId();

    gEncounterMode = ENCOUNTER_FIRERED;

    landMonsInfo = gWildMonHeadersFRLG[headerId].landMonsInfo;
    waterMonsInfo = gWildMonHeadersFRLG[headerId].waterMonsInfo;

    switch (Random() % 2)
    {
        case 1:
            oppositeHeaderId = GetCorrespondingLeafGreenWildMonHeaderId(headerId);

            if (oppositeHeaderId != HEADER_NONE)
            {
                gEncounterMode = ENCOUNTER_LEAFGREEN;
                landMonsInfo = gWildMonHeadersFRLG[oppositeHeaderId].landMonsInfo;
                waterMonsInfo = gWildMonHeadersFRLG[oppositeHeaderId].waterMonsInfo;
            }
            break;
        //case 2:
        //    headerId = GetHeliodorWildMonHeaderId();
        //    if (headerId != HEADER_NONE)
        //    {
        //        gEncounterMode = ENCOUNTER_HELIODOR;
        //        landMonsInfo = gWildMonHeaders[headerId].landMonsInfo;
        //        waterMonsInfo = gWildMonHeaders[headerId].waterMonsInfo;
        //    }
        //    break;
    }

    if (headerId == HEADER_NONE)
        return SPECIES_NONE;

    // Neither
    if (landMonsInfo == NULL && waterMonsInfo == NULL)
        return SPECIES_NONE;
    // Land Pokémon
    else if (landMonsInfo != NULL && waterMonsInfo == NULL)
        return landMonsInfo->wildPokemon[ChooseWildMonIndex_Land()].species;
    // Water Pokémon
    else if (landMonsInfo == NULL && waterMonsInfo != NULL)
    {
        *isWaterMon = TRUE;
        return waterMonsInfo->wildPokemon[ChooseWildMonIndex_WaterRock()].species;
    }
    // Either land or water Pokémon
    if ((Random() % 100) < 80)
    {
        return landMonsInfo->wildPokemon[ChooseWildMonIndex_Land()].species;
    }
    else
    {
        *isWaterMon = TRUE;
        return waterMonsInfo->wildPokemon[ChooseWildMonIndex_WaterRock()].species;
    }
}

u32 GetLocalWaterMon(void)
{
    const struct WildPokemonInfo *waterMonsInfo;
    u32 oppositeHeaderId;
    u32 headerId = GetFireRedWildMonHeaderId();

    gEncounterMode = ENCOUNTER_FIRERED;

    waterMonsInfo = gWildMonHeadersFRLG[headerId].waterMonsInfo;

    switch (Random() % 2)
    {
        case 1:
            oppositeHeaderId = GetCorrespondingLeafGreenWildMonHeaderId(headerId);

            if (oppositeHeaderId != HEADER_NONE)
            {
                gEncounterMode = ENCOUNTER_LEAFGREEN;
                waterMonsInfo = gWildMonHeadersFRLG[oppositeHeaderId].waterMonsInfo;
            }
            break;
        //case 2:
        //    headerId = GetHeliodorWildMonHeaderId();
        //    if (headerId != HEADER_NONE)
        //    {
        //        gEncounterMode = ENCOUNTER_HELIODOR;
        //        waterMonsInfo = gWildMonHeaders[headerId].waterMonsInfo;
        //    }
        //    break;
    }

    if (headerId == HEADER_NONE)
        return SPECIES_NONE;

    if (waterMonsInfo)
        return waterMonsInfo->wildPokemon[ChooseWildMonIndex_WaterRock()].species;

    return SPECIES_NONE;
}

bool8 UpdateRepelCounter(void)
{
    u16 steps;

    if (InUnionRoom() == TRUE)
        return FALSE;

    if (gQuestLogState == QL_STATE_PLAYBACK)
        return FALSE;

    steps = VarGet(VAR_REPEL_STEP_COUNT);

    if (steps != 0)
    {
        steps--;
        VarSet(VAR_REPEL_STEP_COUNT, steps);
        if (steps == 0)
        {
            ScriptContext_SetupScript(EventScript_RepelWoreOff);
            return TRUE;
        }
    }
    return FALSE;
}

static bool32 IsWildLevelAllowedByRepel(u32 wildLevel)
{
    u32 i;

    if (!VarGet(VAR_REPEL_STEP_COUNT))
        return TRUE;

    for (i = 0; i < PARTY_SIZE; i++)
    {
        if (GetMonData(&gPlayerParty[i], MON_DATA_HP) && !GetMonData(&gPlayerParty[i], MON_DATA_IS_EGG))
        {
            u32 ourLevel = GetMonData(&gPlayerParty[i], MON_DATA_LEVEL);

            if (wildLevel < ourLevel)
                return FALSE;
            else
                return TRUE;
        }
    }

    return FALSE;
}

static void ApplyFluteEncounterRateMod(u32 *encounterRate)
{
    switch (GetFluteEncounterRateModType())
    {
    case 1:
        *encounterRate += *encounterRate / 2;
        break;
    case 2:
        *encounterRate = *encounterRate / 2;
        break;
    }
}

static u8 GetFluteEncounterRateModType(void)
{
    if (FlagGet(FLAG_SYS_WHITE_FLUTE_ACTIVE) == TRUE)
        return 1;
    else if (FlagGet(FLAG_SYS_BLACK_FLUTE_ACTIVE) == TRUE)
        return 2;
    else
        return 0;
}

static void ApplyCleanseTagEncounterRateMod(u32 *encounterRate)
{
    if (IsLeadMonHoldingCleanseTag())
        *encounterRate = *encounterRate * 2 / 3;
}

static bool8 IsLeadMonHoldingCleanseTag(void)
{
    if (sWildEncounterData.leadMonHeldItem == ITEM_CLEANSE_TAG)
        return TRUE;
    else
        return FALSE;
}

void SeedWildEncounterRng(u16 seed)
{
    sWildEncounterData.rngState = seed;
    ResetEncounterRateModifiers();
}

static inline u32 GetMapBaseEncounterCooldown(u32 encounterType)
{
    const struct WildPokemonInfo *landMonsInfo;
    const struct WildPokemonInfo *waterMonsInfo;
    u32 oppositeHeaderId;
    u32 headerId = GetFireRedWildMonHeaderId();

    gEncounterMode = ENCOUNTER_FIRERED;

    landMonsInfo = gWildMonHeadersFRLG[headerId].landMonsInfo;
    waterMonsInfo = gWildMonHeadersFRLG[headerId].waterMonsInfo;

    switch (Random() % 2)
    {
        case 1:
            oppositeHeaderId = GetCorrespondingLeafGreenWildMonHeaderId(headerId);

            if (oppositeHeaderId != HEADER_NONE)
            {
                gEncounterMode = ENCOUNTER_LEAFGREEN;
                landMonsInfo = gWildMonHeadersFRLG[oppositeHeaderId].landMonsInfo;
                waterMonsInfo = gWildMonHeadersFRLG[oppositeHeaderId].waterMonsInfo;
            }
            break;
        //case 2:
        //    headerId = GetHeliodorWildMonHeaderId();
        //    if (headerId != HEADER_NONE)
        //    {
        //        gEncounterMode = ENCOUNTER_HELIODOR;
        //        waterMonsInfo = gWildMonHeaders[headerId].waterMonsInfo;
        //    }
        //    break;
    }

    if (headerId == HEADER_NONE)
        return 0xFF;
    if (encounterType == TILE_ENCOUNTER_LAND)
    {
        if (landMonsInfo == NULL)
            return 0xFF;
        if (landMonsInfo->encounterRate >= 80)
            return 0;
        if (landMonsInfo->encounterRate < 10)
            return 8;
        return 8 - (landMonsInfo->encounterRate / 10);
    }
    if (encounterType == TILE_ENCOUNTER_WATER)
    {
        if (waterMonsInfo == NULL)
            return 0xFF;
        if (waterMonsInfo->encounterRate >= 80)
            return 0;
        if (waterMonsInfo->encounterRate < 10)
            return 8;
        return 8 - (waterMonsInfo->encounterRate / 10);
    }
    return 0xFF;
}

void ResetEncounterRateModifiers(void)
{
    sWildEncounterData.encounterRateBuff = 0;
    sWildEncounterData.stepsSinceLastEncounter = 0;
}

static u8 GetAbilityEncounterRateModType(void)
{
    sWildEncounterData.abilityEffect = 0;
    if (!GetMonData(&gPlayerParty[0], MON_DATA_SANITY_IS_EGG))
    {
        u8 ability = GetMonAbility(&gPlayerParty[0]);
        if (ability == ABILITY_STENCH)
            sWildEncounterData.abilityEffect = 1;
        else if (ability == ABILITY_ILLUMINATE)
            sWildEncounterData.abilityEffect = 2;
    }
    return sWildEncounterData.abilityEffect;
}

static bool8 HandleWildEncounterCooldown(u32 currMetatileAttrs)
{
    u8 encounterType = ExtractMetatileAttribute(currMetatileAttrs, METATILE_ATTRIBUTE_ENCOUNTER_TYPE);
    u32 minSteps;
    u32 encRate;
    if (encounterType == TILE_ENCOUNTER_NONE)
        return FALSE;
    minSteps = GetMapBaseEncounterCooldown(encounterType);
    if (minSteps == 0xFF)
        return FALSE;
    minSteps *= 256;
    encRate = 5 * 256;
    switch (GetFluteEncounterRateModType())
    {
    case 1:
        minSteps -= minSteps / 2;
        encRate += encRate / 2;
        break;
    case 2:
        minSteps *= 2;
        encRate /= 2;
        break;
    }
    sWildEncounterData.leadMonHeldItem = GetMonData(&gPlayerParty[0], MON_DATA_HELD_ITEM);
    if (IsLeadMonHoldingCleanseTag() == TRUE)
    {
        minSteps += minSteps / 3;
        encRate -= encRate / 3;
    }
    switch (GetAbilityEncounterRateModType())
    {
    case 1:
        minSteps *= 2;
        encRate /= 2;
        break;
    case 2:
        minSteps /= 2;
        encRate *= 2;
        break;
    }
    minSteps /= 256;
    encRate /= 256;
    if (sWildEncounterData.stepsSinceLastEncounter >= minSteps)
        return TRUE;
    sWildEncounterData.stepsSinceLastEncounter++;
    if ((Random() % 100) < encRate)
        return TRUE;
    return FALSE;
}

bool8 TryStandardWildEncounter(u32 currMetatileAttrs)
{
    if (!HandleWildEncounterCooldown(currMetatileAttrs))
    {
        sWildEncounterData.prevMetatileBehavior = ExtractMetatileAttribute(currMetatileAttrs, METATILE_ATTRIBUTE_BEHAVIOR);
        return FALSE;
    }
    else if (StandardWildEncounter(currMetatileAttrs, sWildEncounterData.prevMetatileBehavior) == TRUE)
    {
        sWildEncounterData.encounterRateBuff = 0;
        sWildEncounterData.stepsSinceLastEncounter = 0;
        sWildEncounterData.prevMetatileBehavior = ExtractMetatileAttribute(currMetatileAttrs, METATILE_ATTRIBUTE_BEHAVIOR);
        return TRUE;
    }
    else
    {
        sWildEncounterData.prevMetatileBehavior = ExtractMetatileAttribute(currMetatileAttrs, METATILE_ATTRIBUTE_BEHAVIOR);
        return FALSE;
    }
}

static void AddToWildEncounterRateBuff(u8 encounterRate)
{
    if (VarGet(VAR_REPEL_STEP_COUNT) == 0)
        sWildEncounterData.encounterRateBuff += encounterRate;
    else
        sWildEncounterData.encounterRateBuff = 0;
}

static inline u32 EncounterCore(u32 headerId, u32 curMetatileBehavior, u32 prevMetatileBehavior, const struct WildPokemonInfo *wildPokemonInfo, u32 terrain, bool32 bypassCheck, u32 partySlot)
{
    struct Roamer *roamer;
    u32 i;
    u32 roamerId;
    u32 flags = WILD_CHECK_REPEL;

    if (prevMetatileBehavior != curMetatileBehavior && !AllowWildCheckOnNewMetatile())
        return ENCOUNTER_FAIL;

    if (bypassCheck || WildEncounterCheck(wildPokemonInfo->encounterRate, FALSE, FALSE) == TRUE)
    {
        roamerId = TryStartRoamerEncounter();

        if (roamerId == 1)
        {
            roamer = &gSaveBlock1Ptr->roamer;

            if (!IsWildLevelAllowedByRepel(roamer->level))
                return ENCOUNTER_FAIL;

            return ENCOUNTER_ROAMER_1;
        }
        if (roamerId == 2)
        {
            roamer = &gSaveBlock1Ptr->extraRoamers[0];

            if (!IsWildLevelAllowedByRepel(roamer->level))
                return ENCOUNTER_FAIL;

            return ENCOUNTER_ROAMER_2;
        }
        if (roamerId == 3)
        {
            roamer = &gSaveBlock1Ptr->extraRoamers[1];

            if (!IsWildLevelAllowedByRepel(roamer->level))
                return ENCOUNTER_FAIL;

            return ENCOUNTER_ROAMER_3;
        }
        else
        {
            //if (DoMassOutbreakEncounterTest() == TRUE && SetUpMassOutbreakEncounter(flags, partySlot) == TRUE)
            //    return ENCOUNTER_SWARM;

            if (TryGenerateWildMon(wildPokemonInfo, terrain, flags, partySlot) == TRUE)
                return ENCOUNTER_SUCCESS;

            return ENCOUNTER_FAIL;
        }
    }
    else
    {
        return ENCOUNTER_FAIL;
    }
}

#define FORCE_SPECIES   (1 << 0)
#define FORCE_TYPE      (1 << 1)
#define FORCE_GENDER    (1 << 2)
#define FORCE_NATURE    (1 << 3)
#define FORCE_MAXLEVEL  (1 << 4)

static inline u32 EncounterLoop(u32 headerId, u32 curMetatileBehavior, u32 prevMetatileBehavior, const struct WildPokemonInfo *wildPokemonInfo, u32 terrain, u32 partySlot, u32 forceFlags, u32 species, u32 type, u32 gender, u32 nature, bool32 bypassCheck)
{
    u32 tid = gSaveBlock2Ptr->playerTrainerId[0]
           | (gSaveBlock2Ptr->playerTrainerId[1] << 8)
           | (gSaveBlock2Ptr->playerTrainerId[2] << 16)
           | (gSaveBlock2Ptr->playerTrainerId[3] << 24);

    do
    {
        if (IsShinyOtIdPersonality(tid, sTempMons[partySlot + 1].pid))
            break;
    } while (EncounterCore(headerId, curMetatileBehavior, prevMetatileBehavior, wildPokemonInfo, terrain, bypassCheck, partySlot) != ENCOUNTER_SUCCESS
          || (forceFlags & FORCE_SPECIES && sTempMons[partySlot + 1].species != species)
          || (forceFlags & FORCE_TYPE && (gSpeciesInfo[sTempMons[partySlot + 1].species].types[0] != type && gSpeciesInfo[sTempMons[partySlot + 1].species].types[1] != type))
          || (forceFlags & FORCE_GENDER && GetGenderFromSpeciesAndPersonality(sTempMons[partySlot + 1].species, sTempMons[partySlot + 1].pid) != gender)
          || (forceFlags & FORCE_NATURE && sTempMons[partySlot + 1].pid % NUM_NATURES != nature)
          || (forceFlags & FORCE_MAXLEVEL && sTempMons[partySlot + 1].level != sMaxLevel));

}

static inline u32 GenerateEncounter(u32 headerId, u32 curMetatileBehavior, u32 prevMetatileBehavior, const struct WildPokemonInfo *wildPokemonInfo, u32 terrain, bool32 bypassCheck, u32 partySlot)
{
    u32 result, tableSize, i;
    u32 species = 0;
    u32 type = 0;
    u32 gender = 0;
    u32 nature = 0;
    u32 rolls = 1;
    u32 forceFlags = 0;
    u32 tid = gSaveBlock2Ptr->playerTrainerId[0]
           | (gSaveBlock2Ptr->playerTrainerId[1] << 8)
           | (gSaveBlock2Ptr->playerTrainerId[2] << 16)
           | (gSaveBlock2Ptr->playerTrainerId[3] << 24);

    if (gEncounterMode == ENCOUNTER_RUBY || gEncounterMode == ENCOUNTER_SAPPHIRE)
        tid = (gSaveBlock1Ptr->rubySapphireSecretId << 16) | (tid & 0xFFFF);

    gDisableVBlankRNGAdvance = TRUE;

    sTempMons[0].pid = GetMonData(&gPlayerParty[0], MON_DATA_PERSONALITY);
    sTempMons[0].species = GetMonData(&gPlayerParty[0], MON_DATA_SPECIES);
    sTempMons[0].level = GetMonData(&gPlayerParty[0], MON_DATA_LEVEL);
    sTempMons[0].notEgg = !GetMonData(&gPlayerParty[0], MON_DATA_SANITY_IS_EGG);
    sTempMons[0].item = GetMonData(&gPlayerParty[0], MON_DATA_HELD_ITEM);

    if (sTempMons[0].notEgg)
        sTempMons[0].ability = GetMonAbility(&gPlayerParty[0]);
    else
        sTempMons[0].ability = ABILITY_NONE;

        result = EncounterCore(headerId, curMetatileBehavior, prevMetatileBehavior, wildPokemonInfo, terrain, bypassCheck, partySlot);

    if (result == ENCOUNTER_ROAMER_1 || result == ENCOUNTER_ROAMER_2 || result == ENCOUNTER_ROAMER_3)
    {
        StartRoamerBattle();
    }
    else if (result == ENCOUNTER_SUCCESS)
    {
        u32 version;

        for (i = 0; i < rolls; i++)
        {
            EncounterLoop(headerId, curMetatileBehavior, prevMetatileBehavior, wildPokemonInfo, terrain, partySlot, forceFlags, species, type, gender, nature, bypassCheck);

            if (IsShinyOtIdPersonality(tid, sTempMons[partySlot + 1].pid))
                break;
        }

        switch (gEncounterMode)
        {
            case ENCOUNTER_RUBY:
                version = VERSION_RUBY;
                break;
            case ENCOUNTER_SAPPHIRE:
                version = VERSION_SAPPHIRE;
                break;
            case ENCOUNTER_FIRERED:
                version = VERSION_FIRERED;
                break;
            case ENCOUNTER_LEAFGREEN:
                version = VERSION_LEAFGREEN;
                break;
            case ENCOUNTER_EMERALD:
                version = VERSION_EMERALD;
                break;
            default:
                version = VERSION_COLOXD;    // Shouldn't happen
                break;
        }

        CreateMon(&gEnemyParty[partySlot], sTempMons[partySlot + 1].species, sTempMons[partySlot + 1].level, USE_RANDOM_IVS, TRUE, sTempMons[partySlot + 1].pid, OT_ID_PLAYER_ID, 0);
        SetMonData(&gEnemyParty[partySlot], MON_DATA_HP_IV, &(sTempMons[partySlot + 1].ivs.hp));
        SetMonData(&gEnemyParty[partySlot], MON_DATA_ATK_IV, &(sTempMons[partySlot + 1].ivs.atk));
        SetMonData(&gEnemyParty[partySlot], MON_DATA_DEF_IV, &(sTempMons[partySlot + 1].ivs.def));
        SetMonData(&gEnemyParty[partySlot], MON_DATA_SPEED_IV, &(sTempMons[partySlot + 1].ivs.speed));
        SetMonData(&gEnemyParty[partySlot], MON_DATA_SPATK_IV, &(sTempMons[partySlot + 1].ivs.spAtk));
        SetMonData(&gEnemyParty[partySlot], MON_DATA_SPDEF_IV, &(sTempMons[partySlot + 1].ivs.spDef));
        SetMonData(&gEnemyParty[partySlot], MON_DATA_MET_GAME, &version);
    }

    gDisableVBlankRNGAdvance = FALSE;

    return result;
}