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
	static void GatherNotifies(UAnimMontage* Montage, uint32& NotifyId, TArray<FAnimNotifyProEvent>& Notifies, float StartPosition, float TimeDilation);
	static void HandleHistoricNotifies(TArray<FAnimNotifyProEvent>& Notifies, bool bTriggerNotifiesBeforeStartTime, IPlayMontageProInterface* Interface);
	static void SetupNotifyTimers(IPlayMontageProInterface* Interface, const UWorld* World, TArray<FAnimNotifyProEvent>& Notifies);
	static void ClearNotifyTimers(const UWorld* World, TArray<FAnimNotifyProEvent>& Notifies);
	
	static void BroadcastNotifyEvent(FAnimNotifyProEvent& Event, IPlayMontageProInterface* Interface);
	static void EnsureBroadcastNotifyEvents(EAnimNotifyProEventType EventType, TArray<FAnimNotifyProEvent>& Notifies, IPlayMontageProInterface* Interface);

	static void HandleTimeDilation(IPlayMontageProInterface* Interface, const USkinnedMeshComponent* MeshComp, float& TimeDilation, TArray<FAnimNotifyProEvent>& Notifies);
};
