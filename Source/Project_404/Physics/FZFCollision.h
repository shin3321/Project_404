#pragma once

#include "CoreMinimal.h"

#define CPROFILE_FZFMONSTER TEXT("FZFMonster")
#define CPROFILE_FZFPLAYER TEXT("FZFPlayer")
#define CPROFILE_NOCOLLISION TEXT("NoCollision")
#define CCHANNEL_FZFMONSTER ECC_GameTraceChannel1 // 몬스터
#define CCHANNEL_FZFPLAYER ECC_GameTraceChannel2 // 플레이어
#define CCHANNEL_FZFPLAYER_ATTACK ECC_GameTraceChannel3 // 플레이어 Attack
#define CCHANNEL_FZFMONSTER_ATTACK ECC_GameTraceChannel4 // 몬스터 Attack
#define CCHANNEL_FZFBOSSTRIGGER ECC_GameTraceChannel5 // 보스 함정 트리거

