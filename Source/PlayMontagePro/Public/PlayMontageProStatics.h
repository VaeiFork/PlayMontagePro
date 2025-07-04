// Copyright (c) Jared Taylor

#pragma once

#include "CoreMinimal.h"
#include "PlayMontageTypes.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "PlayMontageProStatics.generated.h"

class IPlayMontageProInterface;
/**
 * @TODO desc
 */
UCLASS()
class PLAYMONTAGEPRO_API UPlayMontageProStatics : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	static void GatherNotifies(UAnimMontage* Montage, uint32& NotifyId, TArray<FAnimNotifyProEvent>& Notifies);
	static void TriggerHistoricNotifies(TArray<FAnimNotifyProEvent>& Notifies, float StartTimeSeconds, bool bTriggerNotifiesBeforeStartTime, IPlayMontageProInterface* Interface);
	static void SetupNotifyTimers(IPlayMontageProInterface* Interface, const UWorld* World, TArray<FAnimNotifyProEvent>& Notifies);
	
	static void BroadcastNotifyEvent(FAnimNotifyProEvent& Event, IPlayMontageProInterface* Interface);
	static void EnsureBroadcastNotifyEvents(EAnimNotifyProEventType EventType, TArray<FAnimNotifyProEvent>& Notifies, IPlayMontageProInterface* Interface);
};
