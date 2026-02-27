#ifndef GUARD_WILD_ENCOUNTER_H
#define GUARD_WILD_ENCOUNTER_H

#include "global.h"

#define LAND_WILD_COUNT     12
#define WATER_WILD_COUNT    5
#define ROCK_WILD_COUNT     5
#define FISH_WILD_COUNT     10

#define NUM_ALTERING_CAVE_TABLES 9

struct WildPokemon
{
    u8 minLevel;
    u8 maxLevel;
    u16 species;
    u32 originGame;
    u32 encounterRate;
};

struct WildPokemonInfo
{
    u8 encounterRate;
    const struct WildPokemon *wildPokemon;
};

struct WildPokemonHeader
{
    u8 mapGroup;
    u8 mapNum;
    const struct WildPokemonInfo *landMonsInfo;
    const struct WildPokemonInfo *waterMonsInfo;
    const struct WildPokemonInfo *rockSmashMonsInfo;
    const struct WildPokemonInfo *fishingMonsInfo;
};

struct TempMon
{
    u32 pid;
    struct IVs ivs;
    u32 species;
    u32 level;
    bool32 notEgg;
    u32 ability;
    u32 item;
};

extern const struct WildPokemonHeader gWildMonHeadersFRLG[];

void DisableWildEncounters(bool8 disabled);
bool32 StandardWildEncounter(u32 curMetatileBehavior, u32 prevMetatileBehavior);
bool32 SweetScentWildEncounter(void);
bool32 DoesCurrentMapHaveFishingMons(void);
void FishingWildEncounter(u32 rod);
u32 GetLocalWildMon(bool8 *isWaterMon);
u32 GetLocalWaterMon(void);
bool8 UpdateRepelCounter(void);
void DisableWildEncounters(bool8 state);
u8 GetUnownLetterByPersonalityLoByte(u32 personality);
void SeedWildEncounterRng(u16 randVal);
void ResetEncounterRateModifiers(void);
bool8 TryStandardWildEncounter(u32 currMetatileAttrs);

#endif // GUARD_WILD_ENCOUNTER_H
