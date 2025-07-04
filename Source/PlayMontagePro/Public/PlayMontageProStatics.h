// Copyright (c) Jared Taylor

#pragma once

#include "CoreMinimal.h"
#include "PlayMontageTypes.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "PlayMontageProStatics.generated.h"

class IPlayMontageProInterface;
/**
 * Common utility functions for PlayMontagePro shared between different PlayMontage nodes.
 */
UCLASS()
class PLAYMONTAGEPRO_API UPlayMontageProStatics : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	/**
	 * Gathers notifies from the montage and returns them in the Notifies array.
	 * @param Montage The montage to gather notifies from.
	 * @param NotifyId The current notify ID, which will be incremented for each notify found.
	 * @param Notifies The array to store the gathered notifies.
	 * @param Section The section of the montage to gather notifies from.
	 * @param StartPosition The starting position of the montage, used to calculate notify times.
	 * @param TimeDilation The time dilation factor to apply to the notify times.
	 */
	static void GatherNotifies(UAnimMontage* Montage, uint32& NotifyId, TArray<FAnimNotifyProEvent>& Notifies, const FName& Section, float StartPosition, float TimeDilation);

	/**
	 * Handles historic notifies, triggering them before the start time if specified, or marking them as skipped.
	 * @param Notifies The array of notifies to handle.
	 * @param bTriggerNotifiesBeforeStartTime Whether to trigger notifies before the start time.
	 * @param Interface The interface to use for broadcasting notify events.
	 */
	static void HandleHistoricNotifies(TArray<FAnimNotifyProEvent>& Notifies, bool bTriggerNotifiesBeforeStartTime, IPlayMontageProInterface* Interface);

	/**
	 * Sets up timers for the notifies in the Notifies array, using the provided interface to create timer delegates.
	 * @param Interface The interface to use for creating timer delegates.
	 * @param World The world context to use for setting up timers.
	 * @param Notifies The array of notifies to set up timers for.
	 */
	static void SetupNotifyTimers(IPlayMontageProInterface* Interface, const UWorld* World, TArray<FAnimNotifyProEvent>& Notifies);

	/**
	 * Clears the timers for the notifies in the Notifies array.
	 * @param World The world context to use for clearing timers.
	 * @param Notifies The array of notifies to clear timers for.
	 */
	static void ClearNotifyTimers(const UWorld* World, TArray<FAnimNotifyProEvent>& Notifies);

	/**
	 * Broadcasts a notify event using the provided interface.
	 * @param Event The notify event to broadcast.
	 * @param Interface The interface to use for broadcasting the event.
	 */
	static void BroadcastNotifyEvent(FAnimNotifyProEvent& Event, IPlayMontageProInterface* Interface);

	/**
	 * Ensures that broadcast notify events are triggered for the specified event type.
	 * @param EventType The type of event to ensure is broadcasted.
	 * @param Notifies The array of notifies to check and broadcast.
	 * @param Interface The interface to use for broadcasting the events.
	 */
	static void EnsureBroadcastNotifyEvents(EAnimNotifyProEventType EventType, TArray<FAnimNotifyProEvent>& Notifies, IPlayMontageProInterface* Interface);

	/**
	 * Handles time dilation for the montage, adjusting the TimeDilation factor and triggering notifies as needed.
	 * Requires that the mesh component is ticking pose.
	 * @param Interface The interface to use for broadcasting notify events.
	 * @param MeshComp The skinned mesh component associated with the montage.
	 * @param TimeDilation The current time dilation factor to adjust.
	 * @param Notifies The array of notifies to handle.
	 */
	static void HandleTimeDilation(IPlayMontageProInterface* Interface, const USkinnedMeshComponent* MeshComp, float& TimeDilation, TArray<FAnimNotifyProEvent>& Notifies);
};
