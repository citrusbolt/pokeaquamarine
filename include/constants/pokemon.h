#ifndef GUARD_CONSTANTS_POKEMON_H
#define GUARD_CONSTANTS_POKEMON_H

// For (Set|Get)(Box)?MonData
#define MON_DATA_PERSONALITY                0
#define MON_DATA_OT_ID                      1
#define MON_DATA_NICKNAME                   2
#define MON_DATA_LANGUAGE                   3
#define MON_DATA_SANITY_IS_BAD_EGG          4
#define MON_DATA_SANITY_HAS_SPECIES         5
#define MON_DATA_SANITY_IS_EGG              6
#define MON_DATA_BLOCK_BOX_RS               7
#define MON_DATA_OT_NAME                    8
#define MON_DATA_MARKINGS                   9
#define MON_DATA_CHECKSUM                  10
#define MON_DATA_MINT                      11
#define MON_DATA_FORM                      12
#define MON_DATA_HP_HT                     13
#define MON_DATA_ATK_HT                    14
#define MON_DATA_DEF_HT                    15
#define MON_DATA_SPEED_HT                  16
#define MON_DATA_SPATK_HT                  17
#define MON_DATA_SPDEF_HT                  18
#define MON_DATA_ENCRYPT_SEPARATOR         19
#define MON_DATA_SPECIES                   20
#define MON_DATA_HELD_ITEM                 21
#define MON_DATA_MOVE1                     22
#define MON_DATA_MOVE2                     23
#define MON_DATA_MOVE3                     24
#define MON_DATA_MOVE4                     25
#define MON_DATA_PP1                       26
#define MON_DATA_PP2                       27
#define MON_DATA_PP3                       28
#define MON_DATA_PP4                       29
#define MON_DATA_PP_BONUSES                30
#define MON_DATA_COOL                      31
#define MON_DATA_BEAUTY                    32
#define MON_DATA_CUTE                      33
#define MON_DATA_EXP                       34
#define MON_DATA_HP_EV                     35
#define MON_DATA_ATK_EV                    36
#define MON_DATA_DEF_EV                    37
#define MON_DATA_SPEED_EV                  38
#define MON_DATA_SPATK_EV                  39
#define MON_DATA_SPDEF_EV                  40
#define MON_DATA_FRIENDSHIP                41
#define MON_DATA_SMART                     42
#define MON_DATA_POKERUS                   43
#define MON_DATA_MET_LOCATION              44
#define MON_DATA_MET_LEVEL                 45
#define MON_DATA_MET_GAME                  46
#define MON_DATA_POKEBALL                  47
#define MON_DATA_HP_IV                     48
#define MON_DATA_ATK_IV                    49
#define MON_DATA_DEF_IV                    50
#define MON_DATA_SPEED_IV                  51
#define MON_DATA_SPATK_IV                  52
#define MON_DATA_SPDEF_IV                  53
#define MON_DATA_IS_EGG                    54
#define MON_DATA_ABILITY_NUM               55
#define MON_DATA_TOUGH                     56
#define MON_DATA_SHEEN                     57
#define MON_DATA_OT_GENDER                 58
#define MON_DATA_COOL_RIBBON               59
#define MON_DATA_BEAUTY_RIBBON             60
#define MON_DATA_CUTE_RIBBON               61
#define MON_DATA_SMART_RIBBON              62
#define MON_DATA_TOUGH_RIBBON              63
#define MON_DATA_STATUS                    64
#define MON_DATA_LEVEL                     65
#define MON_DATA_HP                        66
#define MON_DATA_MAX_HP                    67
#define MON_DATA_ATK                       68
#define MON_DATA_DEF                       69
#define MON_DATA_SPEED                     70
#define MON_DATA_SPATK                     71
#define MON_DATA_SPDEF                     72
#define MON_DATA_MAIL                      73
#define MON_DATA_SPECIES_OR_EGG            74
#define MON_DATA_IVS                       75
#define MON_DATA_CHAMPION_RIBBON           76
#define MON_DATA_WINNING_RIBBON            77
#define MON_DATA_VICTORY_RIBBON            78
#define MON_DATA_ARTIST_RIBBON             79
#define MON_DATA_EFFORT_RIBBON             80
#define MON_DATA_MARINE_RIBBON             81
#define MON_DATA_LAND_RIBBON               82
#define MON_DATA_SKY_RIBBON                83
#define MON_DATA_COUNTRY_RIBBON            84
#define MON_DATA_NATIONAL_RIBBON           85
#define MON_DATA_EARTH_RIBBON              86
#define MON_DATA_WORLD_RIBBON              87
#define MON_DATA_UNUSED_RIBBONS            88
#define MON_DATA_MODERN_FATEFUL_ENCOUNTER  89
#define MON_DATA_KNOWN_MOVES               90
#define MON_DATA_RIBBON_COUNT              91
#define MON_DATA_RIBBONS                   92
#define MON_DATA_ATK2                      93
#define MON_DATA_DEF2                      94
#define MON_DATA_SPEED2                    95
#define MON_DATA_SPATK2                    96
#define MON_DATA_SPDEF2                    97
#define MON_DATA_VERSION_MODIFIER          98
#define MON_DATA_TITLE                     99
#define MON_DATA_TITLE_STRING             100

// Pokemon types
#define TYPE_NONE           255
#define TYPE_NORMAL         0
#define TYPE_FIGHTING       1
#define TYPE_FLYING         2
#define TYPE_POISON         3
#define TYPE_GROUND         4
#define TYPE_ROCK           5
#define TYPE_BUG            6
#define TYPE_GHOST          7
#define TYPE_STEEL          8
#define TYPE_MYSTERY        9
#define TYPE_FIRE           10
#define TYPE_WATER          11
#define TYPE_GRASS          12
#define TYPE_ELECTRIC       13
#define TYPE_PSYCHIC        14
#define TYPE_ICE            15
#define TYPE_DRAGON         16
#define TYPE_DARK           17
#define NUMBER_OF_MON_TYPES 18

// Pokemon egg groups
#define EGG_GROUP_NONE               0
#define EGG_GROUP_MONSTER            1
#define EGG_GROUP_WATER_1            2
#define EGG_GROUP_BUG                3
#define EGG_GROUP_FLYING             4
#define EGG_GROUP_FIELD              5
#define EGG_GROUP_FAIRY              6
#define EGG_GROUP_GRASS              7
#define EGG_GROUP_HUMAN_LIKE         8
#define EGG_GROUP_WATER_3            9
#define EGG_GROUP_MINERAL            10
#define EGG_GROUP_AMORPHOUS          11
#define EGG_GROUP_WATER_2            12
#define EGG_GROUP_DITTO              13
#define EGG_GROUP_DRAGON             14
#define EGG_GROUP_NO_EGGS_DISCOVERED 15

#define EGG_GROUPS_PER_MON      2

// Pokemon natures
#define NATURE_HARDY    0
#define NATURE_LONELY   1
#define NATURE_BRAVE    2
#define NATURE_ADAMANT  3
#define NATURE_NAUGHTY  4
#define NATURE_BOLD     5
#define NATURE_DOCILE   6
#define NATURE_RELAXED  7
#define NATURE_IMPISH   8
#define NATURE_LAX      9
#define NATURE_TIMID    10
#define NATURE_HASTY    11
#define NATURE_SERIOUS  12
#define NATURE_JOLLY    13
#define NATURE_NAIVE    14
#define NATURE_MODEST   15
#define NATURE_MILD     16
#define NATURE_QUIET    17
#define NATURE_BASHFUL  18
#define NATURE_RASH     19
#define NATURE_CALM     20
#define NATURE_GENTLE   21
#define NATURE_SASSY    22
#define NATURE_CAREFUL  23
#define NATURE_QUIRKY   24
#define NUM_NATURES     25

// Pokemon Stats
#define STAT_HP      0
#define STAT_ATK     1
#define STAT_DEF     2
#define STAT_SPEED   3
#define STAT_SPATK   4
#define STAT_SPDEF   5
#define NUM_STATS    6

#define STAT_ACC     6 // Only in battles.
#define STAT_EVASION 7 // Only in battles.

#define NUM_NATURE_STATS (NUM_STATS - 1) // excludes HP
#define NUM_BATTLE_STATS (NUM_STATS + 2) // includes Accuracy and Evasion

#define MIN_STAT_STAGE     0
#define DEFAULT_STAT_STAGE 6
#define MAX_STAT_STAGE    12

// Shiny odds
#define SHINY_ODDS 8 // Actual probability is SHINY_ODDS/65536
#define MASUDA_METHOD_REROLLS 5
#define SHINY_CHARM_REROLLS   2

#define MAX_LEVEL 100

#define OT_ID_PLAYER_ID       0
#define OT_ID_PRESET          1
#define OT_ID_RANDOM_NO_SHINY 2

#define MON_GIVEN_TO_PARTY 0
#define MON_GIVEN_TO_PC    1
#define MON_CANT_GIVE      2

#define PLAYER_HAS_TWO_USABLE_MONS 0
#define PLAYER_HAS_ONE_MON         1
#define PLAYER_HAS_ONE_USABLE_MON  2

// Learning moves
#define MON_ALREADY_KNOWS_MOVE 0xFFFE
#define MON_HAS_MAX_MOVES      0xFFFF

#define LEVEL_UP_MOVE_ID   0x01FF
#define LEVEL_UP_MOVE_LV   0xFE00
#define LEVEL_UP_MOVE_BABY 0x10000
#define LEVEL_UP_END       0xFFFF

#define MAX_LEVEL_UP_MOVES       20

#define MON_MALE       0x00
#define MON_FEMALE     0xFE
#define MON_GENDERLESS 0xFF

#define FRIENDSHIP_EVENT_GROW_LEVEL           0
#define FRIENDSHIP_EVENT_VITAMIN              1
#define FRIENDSHIP_EVENT_BATTLE_ITEM          2
#define FRIENDSHIP_EVENT_LEAGUE_BATTLE        3
#define FRIENDSHIP_EVENT_LEARN_TMHM           4
#define FRIENDSHIP_EVENT_WALKING              5
#define FRIENDSHIP_EVENT_MASSAGE              6
#define FRIENDSHIP_EVENT_FAINT_SMALL          7
#define FRIENDSHIP_EVENT_FAINT_OUTSIDE_BATTLE 8
#define FRIENDSHIP_EVENT_FAINT_LARGE          9

// Friendship value that the majority of species use. This was changed in Generation 8 to 50.
#define STANDARD_FRIENDSHIP 70

#define MAX_FRIENDSHIP  255
#define MAX_SHEEN       255
#define MAX_CONDITION   255

#define MAX_PER_STAT_IVS 31
#define MAX_IV_MASK 31
#define USE_RANDOM_IVS (MAX_PER_STAT_IVS + 1)
#define MAX_PER_STAT_EVS 252
#define MAX_TOTAL_EVS 510
#define EV_ITEM_RAISE_LIMIT 252

// Battle move flags
#define FLAG_MAKES_CONTACT          (1 << 0)
#define FLAG_PROTECT_AFFECTED       (1 << 1)
#define FLAG_MAGIC_COAT_AFFECTED    (1 << 2)
#define FLAG_SNATCH_AFFECTED        (1 << 3)
#define FLAG_MIRROR_MOVE_AFFECTED   (1 << 4)
#define FLAG_KINGS_ROCK_AFFECTED    (1 << 5)

// Growth rates
#define GROWTH_MEDIUM_FAST  0
#define GROWTH_ERRATIC      1
#define GROWTH_FLUCTUATING  2
#define GROWTH_MEDIUM_SLOW  3
#define GROWTH_FAST         4
#define GROWTH_SLOW         5

// Body colors for pokedex search
#define BODY_COLOR_RED      0
#define BODY_COLOR_BLUE     1
#define BODY_COLOR_YELLOW   2
#define BODY_COLOR_GREEN    3
#define BODY_COLOR_BLACK    4
#define BODY_COLOR_BROWN    5
#define BODY_COLOR_PURPLE   6
#define BODY_COLOR_GRAY     7
#define BODY_COLOR_WHITE    8
#define BODY_COLOR_PINK     9

// Evolution types
#define EVO_FRIENDSHIP       1  // Pokémon levels up with friendship ≥ 220
#define EVO_FRIENDSHIP_DAY   2  // Pokémon levels up during the day with friendship ≥ 220
#define EVO_FRIENDSHIP_NIGHT 3  // Pokémon levels up at night with friendship ≥ 220
#define EVO_LEVEL            4  // Pokémon reaches the specified level
#define EVO_TRADE            5  // Pokémon is traded
#define EVO_TRADE_ITEM       6  // Pokémon is traded while it's holding the specified item
#define EVO_ITEM             7  // specified item is used on Pokémon
#define EVO_LEVEL_ATK_GT_DEF 8  // Pokémon reaches the specified level with attack > defense
#define EVO_LEVEL_ATK_EQ_DEF 9  // Pokémon reaches the specified level with attack = defense
#define EVO_LEVEL_ATK_LT_DEF 10 // Pokémon reaches the specified level with attack < defense
#define EVO_LEVEL_SILCOON    11 // Pokémon reaches the specified level with a Silcoon personality value
#define EVO_LEVEL_CASCOON    12 // Pokémon reaches the specified level with a Cascoon personality value
#define EVO_LEVEL_NINJASK    13 // Pokémon reaches the specified level (special value for Ninjask)
#define EVO_LEVEL_SHEDINJA   14 // Pokémon reaches the specified level (special value for Shedinja)
#define EVO_BEAUTY           15 // Pokémon levels up with beauty ≥ specified value

#define EVOS_PER_MON 5

#define EVO_MODE_NORMAL     0
#define EVO_MODE_TRADE      1
#define EVO_MODE_ITEM_USE   2
#define EVO_MODE_ITEM_CHECK 3 // If an Everstone is being held, still want to show that the stone *could* be used on that Pokémon to evolve

#define MON_PIC_WIDTH 64
#define MON_PIC_HEIGHT 64
#define MON_PIC_SIZE (MON_PIC_WIDTH * MON_PIC_HEIGHT / 2)

// Most pokemon have 1 frame. There are 2 exceptions:
// - Castform has 4 frames, 1 for each form
// - Deoxys has 2 frames, 1 for each form
#define MAX_MON_PIC_FRAMES 4

// used by ShouldIgnoreDeoxysForm
#define DEOXYS_CHECK_BATTLE_SPRITE      1
#define DEOXYS_CHECK_TRADE_MAIN         3
#define DEOXYS_CHECK_BATTLE_ANIM        5

#define ENCOUNTER_RUBY      0
#define ENCOUNTER_SAPPHIRE  1
#define ENCOUNTER_FIRERED   2
#define ENCOUNTER_LEAFGREEN 3
#define ENCOUNTER_EMERALD   4
#define ENCOUNTER_HELIODOR  5
#define ENCOUNTER_RS        6
#define ENCOUNTER_FRLG      7

#define ENCOUNTER_FAIL    0
#define ENCOUNTER_SUCCESS 1
#define ENCOUNTER_ROAMER_1 2
#define ENCOUNTER_ROAMER_2 3
#define ENCOUNTER_ROAMER_3 4
#define ENCOUNTER_SWARM   5

#define MINT_NONE       0
#define MINT_LONELY     1
#define MINT_ADAMANT    2
#define MINT_NAUGHTY    3
#define MINT_BRAVE      4
#define MINT_BOLD       5
#define MINT_IMPISH     6
#define MINT_LAX        7
#define MINT_RELAXED    8
#define MINT_MODEST     9
#define MINT_MILD       10
#define MINT_RASH       11
#define MINT_QUIET      12
#define MINT_CALM       13
#define MINT_GENTLE     14
#define MINT_CAREFUL    15
#define MINT_SASSY      16
#define MINT_TIMID      17
#define MINT_HASTY      18
#define MINT_JOLLY      19
#define MINT_NAIVE      20
#define MINT_SERIOUS    21

#endif // GUARD_CONSTANTS_POKEMON_H
