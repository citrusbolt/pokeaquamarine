#ifndef GUARD_ROAMER_H
#define GUARD_ROAMER_H

#include "global.h"

void ClearRoamerData(void);
void ClearRoamerLocationData(void);
void InitRoamer(void);
void UpdateLocationHistoryForRoamer(void);
void RoamerMoveToOtherLocationSet(void);
void RoamerMove(void);
bool8 IsRoamerAt(u8 mapGroup, u8 mapNum);
void CreateRoamerMonInstance(void);
u8 TryStartRoamerEncounter(void);
void UpdateRoamerHPStatus(struct Pokemon *mon);
void SetRoamerInactive(struct Pokemon *mon);
void GetRoamerLocation(u8 *mapGroup, u8 *mapNum, u32 id);
u16 GetRoamerLocationMapSectionId(u32 species);

#endif // GUARD_ROAMER_H
