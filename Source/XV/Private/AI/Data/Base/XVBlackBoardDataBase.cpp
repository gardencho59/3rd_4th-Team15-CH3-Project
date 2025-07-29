#include "AI/Data/Base/XVBlackBoardDataBase.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Float.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Vector.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Bool.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Object.h"

UXVBlackBoardDataBase::UXVBlackBoardDataBase()
{
    FBlackboardEntry Entry;
    TObjectPtr<UBlackboardKeyType> KeyType;
    
    // StatusData 관련 키 등록
    Entry.EntryName = TEXT("CurrentSpeed");
    KeyType = UBlackboardKeyType_Float::StaticClass()->GetDefaultObject<UBlackboardKeyType>();
    Entry.KeyType = KeyType;
    Keys.Add(Entry);
    
    Entry.EntryName = TEXT("Damage");
    KeyType = UBlackboardKeyType_Float::StaticClass()->GetDefaultObject<UBlackboardKeyType>();
    Entry.KeyType = KeyType;
    Keys.Add(Entry);
    
    Entry.EntryName = TEXT("CurrentHealth");
    KeyType = UBlackboardKeyType_Float::StaticClass()->GetDefaultObject<UBlackboardKeyType>();
    Entry.KeyType = KeyType;
    Keys.Add(Entry);
    
    Entry.EntryName = TEXT("MaxHealth");
    KeyType = UBlackboardKeyType_Float::StaticClass()->GetDefaultObject<UBlackboardKeyType>();
    Entry.KeyType = KeyType;
    Keys.Add(Entry);

    // SightConfig 관련 키 등록
    Entry.EntryName = TEXT("SightRadius");
    KeyType = UBlackboardKeyType_Float::StaticClass()->GetDefaultObject<UBlackboardKeyType>();
    Entry.KeyType = KeyType;
    Keys.Add(Entry);
    
    Entry.EntryName = TEXT("LoseSightRadius");
    KeyType = UBlackboardKeyType_Float::StaticClass()->GetDefaultObject<UBlackboardKeyType>();
    Entry.KeyType = KeyType;
    Keys.Add(Entry);
    
    Entry.EntryName = TEXT("PeripheralVisionAngle");
    KeyType = UBlackboardKeyType_Float::StaticClass()->GetDefaultObject<UBlackboardKeyType>();
    Entry.KeyType = KeyType;
    Keys.Add(Entry);
    
    Entry.EntryName = TEXT("SightSetMaxAge");
    KeyType = UBlackboardKeyType_Float::StaticClass()->GetDefaultObject<UBlackboardKeyType>();
    Entry.KeyType = KeyType;
    Keys.Add(Entry);

    // HearingConfig 관련 키 등록
    Entry.EntryName = TEXT("HearingRange");
    KeyType = UBlackboardKeyType_Float::StaticClass()->GetDefaultObject<UBlackboardKeyType>();
    Entry.KeyType = KeyType;
    Keys.Add(Entry);
    
    Entry.EntryName = TEXT("HearingSetMaxAge");
    KeyType = UBlackboardKeyType_Float::StaticClass()->GetDefaultObject<UBlackboardKeyType>();
    Entry.KeyType = KeyType;
    Keys.Add(Entry);

    // GeneralConfig 관련 키 등록
    Entry.EntryName = TEXT("DetectEnemies");
    KeyType = UBlackboardKeyType_Bool::StaticClass()->GetDefaultObject<UBlackboardKeyType>();
    Entry.KeyType = KeyType;
    Keys.Add(Entry);
    
    Entry.EntryName = TEXT("DetectNeutrals");
    KeyType = UBlackboardKeyType_Bool::StaticClass()->GetDefaultObject<UBlackboardKeyType>();
    Entry.KeyType = KeyType;
    Keys.Add(Entry);
    
    Entry.EntryName = TEXT("DetectFriendlies");
    KeyType = UBlackboardKeyType_Bool::StaticClass()->GetDefaultObject<UBlackboardKeyType>();
    Entry.KeyType = KeyType;
    Keys.Add(Entry);

    Entry.EntryName = TEXT("TargetActor");
    auto ObjectKeyType = UBlackboardKeyType_Object::StaticClass()->GetDefaultObject<UBlackboardKeyType_Object>();
    ObjectKeyType->BaseClass = AActor::StaticClass();  // TODO : 베이스 클래스를 Actor로 설정. 추후 플레이어 캐릭터로 변경해야함.
    Entry.KeyType = ObjectKeyType;
    Keys.Add(Entry);


    
    // Location 관련 키 등록
    Entry.EntryName = TEXT("TargetLocation");
    KeyType = UBlackboardKeyType_Vector::StaticClass()->GetDefaultObject<UBlackboardKeyType>();
    Entry.KeyType = KeyType;
    Keys.Add(Entry);
    
    Entry.EntryName = TEXT("LastTargetLocation");
    KeyType = UBlackboardKeyType_Vector::StaticClass()->GetDefaultObject<UBlackboardKeyType>();
    Entry.KeyType = KeyType;
    Keys.Add(Entry);
    
    Entry.EntryName = TEXT("MyLocation");
    KeyType = UBlackboardKeyType_Vector::StaticClass()->GetDefaultObject<UBlackboardKeyType>();
    Entry.KeyType = KeyType;
    Keys.Add(Entry);

    // Perception 관련 Bool 키 등록
    Entry.EntryName = TEXT("CanSeeTarget");
    KeyType = UBlackboardKeyType_Bool::StaticClass()->GetDefaultObject<UBlackboardKeyType>();
    Entry.KeyType = KeyType;
    Keys.Add(Entry);
    
    Entry.EntryName = TEXT("IsInvestigating");
    KeyType = UBlackboardKeyType_Bool::StaticClass()->GetDefaultObject<UBlackboardKeyType>();
    Entry.KeyType = KeyType;
    Keys.Add(Entry);
    
    Entry.EntryName = TEXT("AIIsAttacking");
    KeyType = UBlackboardKeyType_Bool::StaticClass()->GetDefaultObject<UBlackboardKeyType>();
    Entry.KeyType = KeyType;
    Keys.Add(Entry);
}
