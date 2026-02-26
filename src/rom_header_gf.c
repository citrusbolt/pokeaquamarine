#include "global.h"
#include "data.h"
#include "pokemon_icon.h"
#include "decoration.h"
#include "battle_main.h"
#include "item.h"
#include "pokeball.h"
#include "pokedex.h"

struct GFRomHeader
{
    u32 version;
    u32 language;
    u8 gameName[32];
    const struct CompressedSpriteSheet * monFrontPics;
    const struct CompressedSpriteSheet * monBackPics;
    const struct CompressedSpritePalette * monNormalPalettes;
    const struct CompressedSpritePalette * monShinyPalettes;
    const u8 *const * monIcons;
    const u8 * monIconPaletteIds;
    const struct SpritePalette * monIconPalettes;
    const u8 (* monSpeciesNames)[];
    const u8 (* moveNames)[];
    const struct Decoration * decorations;
    u32 flagsOffset;
    u32 varsOffset;
    u32 pokedexOffset;
    u32 seen1Offset;
    u32 seen2Offset;
    u32 pokedexVar;
    u32 pokedexFlag;
    u32 mysteryGiftFlag;
    u32 pokedexCount;
    u8 playerNameLength;
    u8 trainerNameLength;
    u8 pokemonNameLength1;
    u8 pokemonNameLength2;
    u8 moveNameLength;
    u8 itemNameLength;
    u8 berryNameLength;
    u8 abilityNameLength;
    u8 typeNameLength;
    u8 mapNameLength1;
    u8 mapNameLength2;
    u8 trainerClassNameLength;
    u8 decorationNameLength;
    u8 dexCategoryNameLength;
    u8 endOfStringLength;
    u8 frontierTrainerNameLength;
    u8 easyChatWordLength;
    u32 saveBlock2Size;
    u32 saveBlock1Size;
    u32 partyCountOffset;
    u32 partyOffset;
    u32 warpFlagsOffset;
    u32 trainerIdOffset;
    u32 playerNameOffset;
    u32 playerGenderOffset;
    u32 frontierStatusOffset1;
    u32 frontierStatusOffset2;
    u32 externalEventFlagsOffset;
    u32 externalEventDataOffset;
    u32 blockLinkBoxRS:1;
    u32 blockLinkColoXD:1;
    u32 blockLinkUnused:30;
    const struct SpeciesInfo * speciesInfo;
    const u8 (* abilityNames)[];
    const u8 *const * abilityDescriptions;
    const struct Item * items;
    const struct BattleMove * moves;
    const struct CompressedSpriteSheet * ballGfx;
    const struct CompressedSpritePalette * ballPalettes;
    u32 gcnLinkFlagsOffset;
    u32 gameClearFlag;
    u32 ribbonFlag;
    u8 bagCountItems;
    u8 bagCountKeyItems;
    u8 bagCountPokeballs;
    u8 bagCountTMHMs;
    u8 bagCountBerries;
    u8 pcItemsCount;
    u32 pcItemsOffset;
    u32 giftRibbonsOffset;
    u32 enigmaBerryOffset;
    u32 enigmaBerrySize;
    const u8 * moveDescriptions;
    u32 unknown;
};

// This seems to need to be in the text section for some reason.
// To avoid a changed section attributes warning it's put in a special .text.consts section.
__attribute__((section(".text.consts")))
static const struct GFRomHeader sGFRomHeader = {
    .version = GAME_VERSION,
    .language = GAME_LANGUAGE,
    .gameName = "pokemon green version",
    .monFrontPics = gMonFrontPicTable,
    .monBackPics = gMonBackPicTable,
    .monNormalPalettes = gMonPaletteTable,
    .monShinyPalettes = gMonShinyPaletteTable,
    .monIcons = gMonIconTable,
    .monIconPaletteIds = gMonIconPaletteIndices,
    .monIconPalettes = gMonIconPaletteTable,
    .monSpeciesNames = gSpeciesNames,
    .moveNames = gMoveNames,
    .decorations = gDecorations,
    .flagsOffset = offsetof(struct SaveBlock1, flags),
    .varsOffset = offsetof(struct SaveBlock1, vars),
    .pokedexOffset = offsetof(struct SaveBlock2, pokedex),
    .seen1Offset = offsetof(struct SaveBlock1, seen1),
    .seen2Offset = offsetof(struct SaveBlock1, seen2),
    .pokedexVar = VAR_RS_NATIONAL_DEX - VARS_START,
    .pokedexFlag = FLAG_SYS_RS_NATIONAL_DEX,
    .mysteryGiftFlag = FLAG_SYS_MYSTERY_GIFT_ENABLED,
    .pokedexCount = NATIONAL_DEX_COUNT,
    .playerNameLength = PLAYER_NAME_LENGTH,
    .trainerNameLength = 10,
    .pokemonNameLength1 = POKEMON_NAME_LENGTH,
    .pokemonNameLength2 = POKEMON_NAME_LENGTH,
    .moveNameLength = MOVE_NAME_LENGTH,
    .itemNameLength = ITEM_NAME_LENGTH - 2,
    .berryNameLength = BERRY_NAME_LENGTH,
    .abilityNameLength = ABILITY_NAME_LENGTH,
    .typeNameLength = TYPE_NAME_LENGTH,
    .mapNameLength1 = 16,
    .mapNameLength2 = 18,
    .trainerClassNameLength = 12,
    .decorationNameLength = 15,
    .dexCategoryNameLength = 11,
    .endOfStringLength = 1, // One byte, come on man...
    .frontierTrainerNameLength = PLAYER_NAME_LENGTH,
    .easyChatWordLength = 12,
    .saveBlock2Size = sizeof(struct SaveBlock2),
    .saveBlock1Size = sizeof(struct SaveBlock1),
    .partyCountOffset = offsetof(struct SaveBlock1, playerPartyCount),
    .partyOffset = offsetof(struct SaveBlock1, playerParty),
    .warpFlagsOffset = offsetof(struct SaveBlock2, specialSaveWarpFlags),
    .trainerIdOffset = offsetof(struct SaveBlock2, playerTrainerId),
    .playerNameOffset = offsetof(struct SaveBlock2, playerName),
    .playerGenderOffset = offsetof(struct SaveBlock2, playerGender),
    .frontierStatusOffset1 = offsetof(struct SaveBlock2, unkFlag2),
    .frontierStatusOffset2 = offsetof(struct SaveBlock2, unkFlag2),
    .externalEventFlagsOffset = offsetof(struct SaveBlock1, externalEventFlags),
    .externalEventDataOffset = offsetof(struct SaveBlock1, externalEventData),
    .blockLinkBoxRS = FALSE,
    .blockLinkColoXD = FALSE,
    .blockLinkUnused = 0,
    .speciesInfo = gSpeciesInfo,
    .abilityNames = gAbilityNames,
    .abilityDescriptions = gAbilityDescriptionPointers,
    .items = gItems,
    .moves = gBattleMoves,
    .ballGfx = gBallSpriteSheets,
    .ballPalettes = gBallSpritePalettes,
    .gcnLinkFlagsOffset = offsetof(struct SaveBlock2, gcnLinkFlags),
    .gameClearFlag = FLAG_SYS_GAME_CLEAR,
    .ribbonFlag = FLAG_SYS_RIBBON_GET,
    .bagCountItems = BAG_ITEMS_COUNT,
    .bagCountKeyItems = BAG_KEYITEMS_COUNT,
    .bagCountPokeballs = BAG_POKEBALLS_COUNT,
    .bagCountTMHMs = BAG_TMHM_COUNT,
    .bagCountBerries = BAG_BERRIES_COUNT,
    .pcItemsCount = PC_ITEMS_COUNT,
    .pcItemsOffset = offsetof(struct SaveBlock1, pcItems),
    .giftRibbonsOffset = offsetof(struct SaveBlock1, giftRibbons),
    .enigmaBerryOffset = offsetof(struct SaveBlock1, enigmaBerry),
    .enigmaBerrySize = sizeof(struct EnigmaBerry),
    .moveDescriptions = NULL,
    .unknown = 0xFFFFFFFF, // 0x00000000 in Emerald
};
