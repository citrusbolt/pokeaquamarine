#include "global.h"
#include "random.h"
#include "overworld.h"
#include "field_specials.h"
#include "constants/maps.h"
#include "constants/region_map_sections.h"

// Despite having a variable to track it, the roamer is
// hard-coded to only ever be in map group 3
#define ROAMER_MAP_GROUP 3

enum
{
    MAP_GRP, // map group
    MAP_NUM, // map number
};

#define ROAMER_1 (&gSaveBlock1Ptr->roamer)
#define ROAMER_2 (&gSaveBlock1Ptr->extraRoamers[0])
#define ROAMER_3 (&gSaveBlock1Ptr->extraRoamers[1])

EWRAM_DATA u8 sLocationHistory[3][2] = {};
EWRAM_DATA u8 sRoamerLocation[2][3] = {};

#define ___ MAP_NUM(MAP_UNDEFINED) // For empty spots in the location table

// Note: There are two potential softlocks that can occur with this table if its maps are
//       changed in particular ways. They can be avoided by ensuring the following:
//       - There must be at least 2 location sets that start with a different map,
//         i.e. every location set cannot start with the same map. This is because of
//         the while loop in RoamerMoveToOtherLocationSet.
//       - Each location set must have at least 3 unique maps. This is because of
//         the while loop in RoamerMove. In this loop the first map in the set is
//         ignored, and an additional map is ignored if the roamer was there recently.
//       - Additionally, while not a softlock, it's worth noting that if for any
//         map in the location table there is not a location set that starts with
//         that map then the roamer will be significantly less likely to move away
//         from that map when it lands there.
static const u8 sRoamerLocations[][7] = {
    {MAP_NUM(MAP_ROUTE1), MAP_NUM(MAP_ROUTE2), MAP_NUM(MAP_ROUTE21_NORTH), MAP_NUM(MAP_ROUTE22), ___, ___, ___},
    {MAP_NUM(MAP_ROUTE2), MAP_NUM(MAP_ROUTE1), MAP_NUM(MAP_ROUTE3), MAP_NUM(MAP_ROUTE22), ___, ___, ___},
    {MAP_NUM(MAP_ROUTE3), MAP_NUM(MAP_ROUTE2), MAP_NUM(MAP_ROUTE4), ___, ___, ___, ___},
    {MAP_NUM(MAP_ROUTE4), MAP_NUM(MAP_ROUTE3), MAP_NUM(MAP_ROUTE5), MAP_NUM(MAP_ROUTE9), MAP_NUM(MAP_ROUTE24), ___, ___},
    {MAP_NUM(MAP_ROUTE5), MAP_NUM(MAP_ROUTE4), MAP_NUM(MAP_ROUTE6), MAP_NUM(MAP_ROUTE7), MAP_NUM(MAP_ROUTE8), MAP_NUM(MAP_ROUTE9), MAP_NUM(MAP_ROUTE24)},
    {MAP_NUM(MAP_ROUTE6), MAP_NUM(MAP_ROUTE5), MAP_NUM(MAP_ROUTE7), MAP_NUM(MAP_ROUTE8), MAP_NUM(MAP_ROUTE11), ___, ___},
    {MAP_NUM(MAP_ROUTE7), MAP_NUM(MAP_ROUTE5), MAP_NUM(MAP_ROUTE6), MAP_NUM(MAP_ROUTE8), MAP_NUM(MAP_ROUTE16), ___, ___},
    {MAP_NUM(MAP_ROUTE8), MAP_NUM(MAP_ROUTE5), MAP_NUM(MAP_ROUTE6), MAP_NUM(MAP_ROUTE7), MAP_NUM(MAP_ROUTE10), MAP_NUM(MAP_ROUTE12), ___},
    {MAP_NUM(MAP_ROUTE9), MAP_NUM(MAP_ROUTE4), MAP_NUM(MAP_ROUTE5), MAP_NUM(MAP_ROUTE10), MAP_NUM(MAP_ROUTE24), ___, ___},
    {MAP_NUM(MAP_ROUTE10), MAP_NUM(MAP_ROUTE8), MAP_NUM(MAP_ROUTE9), MAP_NUM(MAP_ROUTE12), ___, ___, ___},
    {MAP_NUM(MAP_ROUTE11), MAP_NUM(MAP_ROUTE6), MAP_NUM(MAP_ROUTE12), ___, ___, ___, ___},
    {MAP_NUM(MAP_ROUTE12), MAP_NUM(MAP_ROUTE10), MAP_NUM(MAP_ROUTE11), MAP_NUM(MAP_ROUTE13), ___, ___, ___},
    {MAP_NUM(MAP_ROUTE13), MAP_NUM(MAP_ROUTE12), MAP_NUM(MAP_ROUTE14), ___, ___, ___, ___},
    {MAP_NUM(MAP_ROUTE14), MAP_NUM(MAP_ROUTE13), MAP_NUM(MAP_ROUTE15), ___, ___, ___, ___},
    {MAP_NUM(MAP_ROUTE15), MAP_NUM(MAP_ROUTE14), MAP_NUM(MAP_ROUTE18), MAP_NUM(MAP_ROUTE19), ___, ___, ___},
    {MAP_NUM(MAP_ROUTE16), MAP_NUM(MAP_ROUTE7), MAP_NUM(MAP_ROUTE17), ___, ___, ___, ___},
    {MAP_NUM(MAP_ROUTE17), MAP_NUM(MAP_ROUTE16), MAP_NUM(MAP_ROUTE18), ___, ___, ___, ___},
    {MAP_NUM(MAP_ROUTE18), MAP_NUM(MAP_ROUTE15), MAP_NUM(MAP_ROUTE17), MAP_NUM(MAP_ROUTE19), ___, ___, ___},
    {MAP_NUM(MAP_ROUTE19), MAP_NUM(MAP_ROUTE15), MAP_NUM(MAP_ROUTE18), MAP_NUM(MAP_ROUTE20), ___, ___, ___},
    {MAP_NUM(MAP_ROUTE20), MAP_NUM(MAP_ROUTE19), MAP_NUM(MAP_ROUTE21_NORTH), ___, ___, ___, ___},
    {MAP_NUM(MAP_ROUTE21_NORTH), MAP_NUM(MAP_ROUTE1), MAP_NUM(MAP_ROUTE20), ___, ___, ___, ___},
    {MAP_NUM(MAP_ROUTE22), MAP_NUM(MAP_ROUTE1), MAP_NUM(MAP_ROUTE2), MAP_NUM(MAP_ROUTE23), ___, ___, ___},
    {MAP_NUM(MAP_ROUTE23), MAP_NUM(MAP_ROUTE22), MAP_NUM(MAP_ROUTE2), ___, ___, ___, ___},
    {MAP_NUM(MAP_ROUTE24), MAP_NUM(MAP_ROUTE4), MAP_NUM(MAP_ROUTE5), MAP_NUM(MAP_ROUTE9), ___, ___, ___},
    {MAP_NUM(MAP_ROUTE25), MAP_NUM(MAP_ROUTE24), MAP_NUM(MAP_ROUTE9), ___, ___, ___, ___},
    {___, ___, ___, ___, ___, ___, ___}
};

#undef ___
#define NUM_LOCATION_SETS (ARRAY_COUNT(sRoamerLocations) - 1)
#define NUM_LOCATIONS_PER_SET (ARRAY_COUNT(sRoamerLocations[0]))

void ClearRoamerData(void)
{
    u32 i;
    *ROAMER_1 = (struct Roamer){};
    *ROAMER_2 = (struct Roamer){};
    *ROAMER_3 = (struct Roamer){};
    sRoamerLocation[MAP_GRP][0] = 0;
    sRoamerLocation[MAP_NUM][0] = 0;
    sRoamerLocation[MAP_GRP][1] = 0;
    sRoamerLocation[MAP_NUM][1] = 0;
    sRoamerLocation[MAP_GRP][2] = 0;
    sRoamerLocation[MAP_NUM][2] = 0;
    for (i = 0; i < ARRAY_COUNT(sLocationHistory); i++)
    {
        sLocationHistory[i][MAP_GRP] = 0;
        sLocationHistory[i][MAP_NUM] = 0;
    }
}

#define GetRoamerSpecies(id) ({\
    u16 species[3];\
    switch (GetStarterSpecies())\
    {\
    default:\
        species[0] = SPECIES_RAIKOU;\
        species[1] = SPECIES_ENTEI;\
        species[2] = SPECIES_SUICUNE;\
        break;\
    case SPECIES_BULBASAUR:\
        species[0] = SPECIES_ENTEI;\
        species[1] = SPECIES_SUICUNE;\
        species[2] = SPECIES_RAIKOU;\
        break;\
    case SPECIES_CHARMANDER:\
        species[0] = SPECIES_SUICUNE;\
        species[1] = SPECIES_RAIKOU;\
        species[2] = SPECIES_ENTEI;\
        break;\
    }\
    species[id];\
})

void CreateInitialRoamerMon(u32 id)
{
    struct Pokemon * mon = &gEnemyParty[0];
    u16 species = GetRoamerSpecies(id);
    CreateMon(mon, species, 50, USE_RANDOM_IVS, FALSE, 0, OT_ID_PLAYER_ID, 0);
    switch (id)
    {
        default:
            ROAMER_1->species = species;
            ROAMER_1->level = 50;
            ROAMER_1->status = 0;
            ROAMER_1->active = TRUE;
            ROAMER_1->ivs = GetMonData(mon, MON_DATA_IVS);
            ROAMER_1->personality = GetMonData(mon, MON_DATA_PERSONALITY);
            ROAMER_1->hp = GetMonData(mon, MON_DATA_MAX_HP);
            ROAMER_1->cool = GetMonData(mon, MON_DATA_COOL);
            ROAMER_1->beauty = GetMonData(mon, MON_DATA_BEAUTY);
            ROAMER_1->cute = GetMonData(mon, MON_DATA_CUTE);
            ROAMER_1->smart = GetMonData(mon, MON_DATA_SMART);
            ROAMER_1->tough = GetMonData(mon, MON_DATA_TOUGH);
            break;
        case 1:
            ROAMER_2->species = species;
            ROAMER_2->level = 50;
            ROAMER_2->status = 0;
            ROAMER_2->active = TRUE;
            ROAMER_2->ivs = GetMonData(mon, MON_DATA_IVS);
            ROAMER_2->personality = GetMonData(mon, MON_DATA_PERSONALITY);
            ROAMER_2->hp = GetMonData(mon, MON_DATA_MAX_HP);
            ROAMER_2->cool = GetMonData(mon, MON_DATA_COOL);
            ROAMER_2->beauty = GetMonData(mon, MON_DATA_BEAUTY);
            ROAMER_2->cute = GetMonData(mon, MON_DATA_CUTE);
            ROAMER_2->smart = GetMonData(mon, MON_DATA_SMART);
            ROAMER_2->tough = GetMonData(mon, MON_DATA_TOUGH);
            break;
        case 2:
            ROAMER_3->species = species;
            ROAMER_3->level = 50;
            ROAMER_3->status = 0;
            ROAMER_3->active = TRUE;
            ROAMER_3->ivs = GetMonData(mon, MON_DATA_IVS);
            ROAMER_3->personality = GetMonData(mon, MON_DATA_PERSONALITY);
            ROAMER_3->hp = GetMonData(mon, MON_DATA_MAX_HP);
            ROAMER_3->cool = GetMonData(mon, MON_DATA_COOL);
            ROAMER_3->beauty = GetMonData(mon, MON_DATA_BEAUTY);
            ROAMER_3->cute = GetMonData(mon, MON_DATA_CUTE);
            ROAMER_3->smart = GetMonData(mon, MON_DATA_SMART);
            ROAMER_3->tough = GetMonData(mon, MON_DATA_TOUGH);
            break;
    }
    sRoamerLocation[MAP_GRP][id] = ROAMER_MAP_GROUP;
    sRoamerLocation[MAP_NUM][id] = sRoamerLocations[Random() % NUM_LOCATION_SETS][0];
}

void InitRoamer(void)
{
    ClearRoamerData();
    CreateInitialRoamerMon(0);
    CreateInitialRoamerMon(1);
    CreateInitialRoamerMon(2);
}

void UpdateLocationHistoryForRoamer(void)
{
    sLocationHistory[2][MAP_GRP] = sLocationHistory[1][MAP_GRP];
    sLocationHistory[2][MAP_NUM] = sLocationHistory[1][MAP_NUM];

    sLocationHistory[1][MAP_GRP] = sLocationHistory[0][MAP_GRP];
    sLocationHistory[1][MAP_NUM] = sLocationHistory[0][MAP_NUM];

    sLocationHistory[0][MAP_GRP] = gSaveBlock1Ptr->location.mapGroup;
    sLocationHistory[0][MAP_NUM] = gSaveBlock1Ptr->location.mapNum;
}

void RoamerMoveToOtherLocationSet(void)
{
    u32 i;
    u8 mapNum = 0;

    for (i = 0; i++; i < 3)
    {
        if (i == 0 && !ROAMER_1->active)
            return;

        if (i == 1 && !ROAMER_3->active)
            return;

        if (i == 2 && !ROAMER_3->active)
            return;

        sRoamerLocation[i][MAP_GRP] = ROAMER_MAP_GROUP;

        // Choose a location set that starts with a map
        // different from the roamer's current map
        while (1)
        {
            mapNum = sRoamerLocations[Random() % NUM_LOCATION_SETS][0];
            if (sRoamerLocation[MAP_NUM][i] != mapNum)
            {
                sRoamerLocation[MAP_NUM][i] = mapNum;
                return;
            }
        }
    }
}


void RoamerMove(void)
{
    u8 locSet = 0;

    if ((Random() % 16) == 0)
    {
        RoamerMoveToOtherLocationSet();
    }
    else
    {
        if (!ROAMER_1->active)
            return;

        while (locSet < NUM_LOCATION_SETS)
        {
            // Find the location set that starts with the roamer's current map
            if (sRoamerLocation[MAP_NUM][0] == sRoamerLocations[locSet][0])
            {
                u8 mapNum;
                while (1)
                {
                    // Choose a new map (excluding the first) within this set
                    // Also exclude a map if a roamer was there 2 moves ago
                    mapNum = sRoamerLocations[locSet][(Random() % (NUM_LOCATIONS_PER_SET - 1)) + 1];
                    if (!(sLocationHistory[2][MAP_GRP] == ROAMER_MAP_GROUP
                       && sLocationHistory[2][MAP_NUM] == mapNum)
                       && mapNum != MAP_NUM(MAP_UNDEFINED))
                        break;
                }
                sRoamerLocation[MAP_NUM][0] = mapNum;
                return;
            }
            locSet++;
        }

        locSet = 0;

        if (!ROAMER_2->active)
            return;

        while (locSet < NUM_LOCATION_SETS)
        {
            // Find the location set that starts with the roamer's current map
            if (sRoamerLocation[MAP_NUM][1] == sRoamerLocations[locSet][0])
            {
                u8 mapNum;
                while (1)
                {
                    // Choose a new map (excluding the first) within this set
                    // Also exclude a map if a roamer was there 2 moves ago
                    mapNum = sRoamerLocations[locSet][(Random() % (NUM_LOCATIONS_PER_SET - 1)) + 1];
                    if (!(sLocationHistory[2][MAP_GRP] == ROAMER_MAP_GROUP
                       && sLocationHistory[2][MAP_NUM] == mapNum)
                       && mapNum != MAP_NUM(MAP_UNDEFINED))
                        break;
                }
                sRoamerLocation[MAP_NUM][1] = mapNum;
                return;
            }
            locSet++;
        }

        locSet = 0;

        if (!ROAMER_3->active)
            return;

        while (locSet < NUM_LOCATION_SETS)
        {
            // Find the location set that starts with the roamer's current map
            if (sRoamerLocation[MAP_NUM][2] == sRoamerLocations[locSet][0])
            {
                u8 mapNum;
                while (1)
                {
                    // Choose a new map (excluding the first) within this set
                    // Also exclude a map if a roamer was there 2 moves ago
                    mapNum = sRoamerLocations[locSet][(Random() % (NUM_LOCATIONS_PER_SET - 1)) + 1];
                    if (!(sLocationHistory[2][MAP_GRP] == ROAMER_MAP_GROUP
                       && sLocationHistory[2][MAP_NUM] == mapNum)
                       && mapNum != MAP_NUM(MAP_UNDEFINED))
                        break;
                }
                sRoamerLocation[MAP_NUM][2] = mapNum;
                return;
            }
            locSet++;
        }
    }
}

u32 IsRoamerAt(u8 mapGroup, u8 mapNum)
{
    if (ROAMER_1->active && mapGroup == sRoamerLocation[MAP_GRP][0] && mapNum == sRoamerLocation[MAP_NUM][0])
        return 1;
    else if (ROAMER_2->active && mapGroup == sRoamerLocation[MAP_GRP][1] && mapNum == sRoamerLocation[MAP_NUM][1])
        return 2;
    else if (ROAMER_3->active && mapGroup == sRoamerLocation[MAP_GRP][2] && mapNum == sRoamerLocation[MAP_NUM][2])
        return 3;
    else
        return 0;
}

void CreateRoamerMonInstance(u32 id)
{
    u32 status;
    struct Pokemon *mon = &gEnemyParty[0];
    ZeroEnemyPartyMons();
    switch (id)
    {
        default:
            CreateMonWithIVsPersonality(mon, ROAMER_1->species, ROAMER_1->level, ROAMER_1->ivs, ROAMER_1->personality);
            status = ROAMER_1->status;
            SetMonData(mon, MON_DATA_STATUS, &status);
            SetMonData(mon, MON_DATA_HP, &ROAMER_1->hp);
            SetMonData(mon, MON_DATA_COOL, &ROAMER_1->cool);
            SetMonData(mon, MON_DATA_BEAUTY, &ROAMER_1->beauty);
            SetMonData(mon, MON_DATA_CUTE, &ROAMER_1->cute);
            SetMonData(mon, MON_DATA_SMART, &ROAMER_1->smart);
            SetMonData(mon, MON_DATA_TOUGH, &ROAMER_1->tough);
            break;
        case 1:
            CreateMonWithIVsPersonality(mon, ROAMER_2->species, ROAMER_2->level, ROAMER_2->ivs, ROAMER_2->personality);
            status = ROAMER_2->status;
            SetMonData(mon, MON_DATA_STATUS, &status);
            SetMonData(mon, MON_DATA_HP, &ROAMER_2->hp);
            SetMonData(mon, MON_DATA_COOL, &ROAMER_2->cool);
            SetMonData(mon, MON_DATA_BEAUTY, &ROAMER_2->beauty);
            SetMonData(mon, MON_DATA_CUTE, &ROAMER_2->cute);
            SetMonData(mon, MON_DATA_SMART, &ROAMER_2->smart);
            SetMonData(mon, MON_DATA_TOUGH, &ROAMER_2->tough);
            break;
        case 2:
            CreateMonWithIVsPersonality(mon, ROAMER_3->species, ROAMER_3->level, ROAMER_3->ivs, ROAMER_3->personality);
            status = ROAMER_3->status;
            SetMonData(mon, MON_DATA_STATUS, &status);
            SetMonData(mon, MON_DATA_HP, &ROAMER_3->hp);
            SetMonData(mon, MON_DATA_COOL, &ROAMER_3->cool);
            SetMonData(mon, MON_DATA_BEAUTY, &ROAMER_3->beauty);
            SetMonData(mon, MON_DATA_CUTE, &ROAMER_3->cute);
            SetMonData(mon, MON_DATA_SMART, &ROAMER_3->smart);
            SetMonData(mon, MON_DATA_TOUGH, &ROAMER_3->tough);
            break;
    }
}

bool8 TryStartRoamerEncounter(void)
{
    u32 id = IsRoamerAt(gSaveBlock1Ptr->location.mapGroup, gSaveBlock1Ptr->location.mapNum);

    if (id > 0 && (Random() % 4) == 0)
    {
        CreateRoamerMonInstance(id);
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}
void UpdateRoamerHPStatus(struct Pokemon *mon)
{
    if (GetRoamerSpecies(2) == GetMonData(mon, MON_DATA_SPECIES))
    {
        ROAMER_3->hp = GetMonData(mon, MON_DATA_HP);
        ROAMER_3->status = GetMonData(mon, MON_DATA_STATUS);
    }
    else if (GetRoamerSpecies(1) == GetMonData(mon, MON_DATA_SPECIES))
    {
        ROAMER_2->hp = GetMonData(mon, MON_DATA_HP);
        ROAMER_2->status = GetMonData(mon, MON_DATA_STATUS);
    }
    else
    {
        ROAMER_1->hp = GetMonData(mon, MON_DATA_HP);
        ROAMER_1->status = GetMonData(mon, MON_DATA_STATUS);
    }

    RoamerMoveToOtherLocationSet();
}

void SetRoamerInactive(struct Pokemon *mon)
{
    if (GetRoamerSpecies(2) == GetMonData(mon, MON_DATA_SPECIES))
        ROAMER_3->active = FALSE;
    else if (GetRoamerSpecies(1) == GetMonData(mon, MON_DATA_SPECIES))
        ROAMER_2->active = FALSE;
    else
        ROAMER_1->active = FALSE;
}

void GetRoamerLocation(u8 *mapGroup, u8 *mapNum, u32 id)
{
    *mapGroup = sRoamerLocation[MAP_GRP][id];
    *mapNum = sRoamerLocation[MAP_NUM][id];
}

u16 GetRoamerLocationMapSectionId(u32 species)
{
    if (GetRoamerSpecies(2) == species)
    {
        if (!ROAMER_3->active)
            return MAPSEC_NONE;
        return Overworld_GetMapHeaderByGroupAndId(sRoamerLocation[MAP_GRP][2], sRoamerLocation[MAP_NUM][2])->regionMapSectionId;
    }
    else if (GetRoamerSpecies(1) == species)
    {
        if (!ROAMER_2->active)
            return MAPSEC_NONE;
        return Overworld_GetMapHeaderByGroupAndId(sRoamerLocation[MAP_GRP][1], sRoamerLocation[MAP_NUM][1])->regionMapSectionId;
    }
    else
    {
        if (!ROAMER_1->active)
            return MAPSEC_NONE;
        return Overworld_GetMapHeaderByGroupAndId(sRoamerLocation[MAP_GRP][0], sRoamerLocation[MAP_NUM][0])->regionMapSectionId;
    }
}
