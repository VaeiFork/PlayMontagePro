// Copyright (c) Jared Taylor


#include "PlayMontageProStatics.h"

#include "AnimNotifyPro.h"
#include "AnimNotifyStatePro.h"
#include "PlayMontageProInterface.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(PlayMontageProStatics)

void UPlayMontageProStatics::GatherNotifies(UAnimMontage* Montage, uint32& NotifyId,
	TArray<FAnimNotifyProEvent>& Notifies, float StartPosition, float TimeDilation)
{
	TRACE_CPUPROFILER_EVENT_SCOPE(UPlayMontageProStatics::GatherNotifies);
	
	Notifies.Reset();
	
	TArray<FAnimNotifyEvent>& MontageNotifies = Montage->Notifies;
	for (FAnimNotifyEvent& MontageNotify : MontageNotifies)
	{
		const float NotifyTime = MontageNotify.GetTime();
		const float StartTime = (NotifyTime - StartPosition) * TimeDilation;

		// Add notify to the list of notifies
		if (UAnimNotifyPro* Notify = MontageNotify.Notify ? Cast<UAnimNotifyPro>(MontageNotify.Notify) : nullptr)
		{
			// Create notify event
			FAnimNotifyProEvent NotifyEvent = { ++NotifyId, Notify->EnsureTriggerNotify,
				EAnimNotifyProType::Notify, StartTime };

			// Cache notify
			NotifyEvent.Notify = Notify;
						
			Notifies.Add(NotifyEvent);
		}

		// Add notify states to the list of notifies
		if (UAnimNotifyStatePro* Notify = MontageNotify.NotifyStateClass ? Cast<UAnimNotifyStatePro>(MontageNotify.NotifyStateClass) : nullptr)
		{
			// Compute end time for the notify end state
			const float EndTime = StartTime + (MontageNotify.GetDuration() * TimeDilation);

			// Start state notify
			FAnimNotifyProEvent& NotifyBeginEvent = Notifies.Add_GetRef({ ++NotifyId, Notify->EnsureTriggerNotify,
				EAnimNotifyProType::NotifyStateBegin, StartTime });

			// End state notify
			FAnimNotifyProEvent& NotifyEndEvent = Notifies.Add_GetRef({ ++NotifyId, Notify->EnsureTriggerNotify,
				EAnimNotifyProType::NotifyStateEnd, EndTime });

			// Cache notify state
			NotifyBeginEvent.NotifyState = Notify;
			NotifyEndEvent.NotifyState = Notify;

			// Mark as end state
			NotifyEndEvent.bIsEndState = true;

			// Pair begin and end states
			NotifyBeginEvent.NotifyStatePair = &NotifyEndEvent;
			NotifyEndEvent.NotifyStatePair = &NotifyBeginEvent;
		}
	}
}

void UPlayMontageProStatics::HandleHistoricNotifies(TArray<FAnimNotifyProEvent>& Notifies,
	bool bTriggerNotifiesBeforeStartTime, IPlayMontageProInterface* Interface)
{
	TRACE_CPUPROFILER_EVENT_SCOPE(UPlayMontageProStatics::TriggerHistoricNotifies);
	
	// Trigger notifies before start time and remove them, if we want to trigger them before the start time
	for (FAnimNotifyProEvent& Notify : Notifies)
	{
		if (Notify.Time <= 0.f)
		{
			if (bTriggerNotifiesBeforeStartTime)
			{
				BroadcastNotifyEvent(Notify, Interface);
			}
			else
			{
				Notify.bNotifySkipped = true;
			}
		}
	}
}

void UPlayMontageProStatics::SetupNotifyTimers(IPlayMontageProInterface* Interface, const UWorld* World,
	TArray<FAnimNotifyProEvent>& Notifies)
{
	TRACE_CPUPROFILER_EVENT_SCOPE(UPlayMontageProStatics::SetupNotifyTimers);
	
	for (FAnimNotifyProEvent& Notify : Notifies)
	{
		// Set up timer for notify
		Notify.TimerDelegate = Interface->CreateTimerDelegate(Notify);
		World->GetTimerManager().SetTimer(Notify.Timer, Notify.TimerDelegate, Notify.Time, false);
	}
}

void UPlayMontageProStatics::ClearNotifyTimers(const UWorld* World, TArray<FAnimNotifyProEvent>& Notifies)
{
	TRACE_CPUPROFILER_EVENT_SCOPE(UPlayMontageProStatics::ForfeitNotifyTimers);
	
	for (FAnimNotifyProEvent& Notify : Notifies)
	{
		if (Notify.Timer.IsValid())
		{
			// Clear the timer for this notify
			World->GetTimerManager().ClearTimer(Notify.Timer);
			Notify.TimerDelegate.Unbind();
			Notify.Timer.Invalidate();
		}
	}
}

void UPlayMontageProStatics::BroadcastNotifyEvent(FAnimNotifyProEvent& Event, IPlayMontageProInterface* Interface)
{
	TRACE_CPUPROFILER_EVENT_SCOPE(UPlayMontageProStatics::BroadcastNotifyEvent);
	
	// Ensure we don't broadcast the same event twice
	if (Event.bHasBroadcast || Event.bNotifySkipped)
	{
		return;
	}

	// Ensure the start state broadcasts first if this is the end state
	if (Event.bIsEndState && Event.NotifyStatePair)
	{
		// If our start state was skipped, we can't broadcast the end state
		if (Event.NotifyStatePair->bNotifySkipped)
		{
			return;
		}

		// Broadcast the start state first
		if (!Event.NotifyStatePair->bHasBroadcast)
		{
			BroadcastNotifyEvent(*Event.NotifyStatePair, Interface);
		}
	}

	// Mark the event as broadcast and clear timers
	Event.bHasBroadcast = true;
	Event.ClearTimers();

	// Broadcast notify callback
	switch (Event.NotifyType)
	{
	case EAnimNotifyProType::Notify:
		if (Event.Notify.IsValid())
		{
			Event.Notify->NotifyCallback(Interface->GetMesh(), Interface->GetMontage());
			Interface->NotifyCallback(Event);
		}
		break;
	case EAnimNotifyProType::NotifyStateBegin:
		if (Event.NotifyState.IsValid())
		{
			Event.NotifyState->NotifyBeginCallback(Interface->GetMesh(), Interface->GetMontage());
			Interface->NotifyBeginCallback(Event);
		}
		break;
	case EAnimNotifyProType::NotifyStateEnd:
		if (Event.NotifyState.IsValid())
		{
			Event.NotifyState->NotifyEndCallback(Interface->GetMesh(), Interface->GetMontage());
			Interface->NotifyEndCallback(Event);
		}
		break;
	}
}

void UPlayMontageProStatics::EnsureBroadcastNotifyEvents(EAnimNotifyProEventType EventType,
	TArray<FAnimNotifyProEvent>& Notifies, IPlayMontageProInterface* Interface)
{
	TRACE_CPUPROFILER_EVENT_SCOPE(UPlayMontageProStatics::EnsureBroadcastNotifyEvents);
	
	for (FAnimNotifyProEvent& Event : Notifies)
	{
		if (Event.bHasBroadcast)
		{
			continue;
		}
		
		// Ensure that notifies are triggered if the montage aborts before they're reached when aborted due to these conditions
		const EAnimNotifyProEventType EventFlags = static_cast<EAnimNotifyProEventType>(Event.EnsureTriggerNotify);
		if (EnumHasAnyFlags(EventFlags, EventType))
		{
			Interface->BroadcastNotifyEvent(Event);
		}
		
		// Ensure that the end state is reached if the start state notify was triggered
		if (Event.bIsEndState && Event.NotifyStatePair && Event.NotifyStatePair->bHasBroadcast)
		{
			Interface->BroadcastNotifyEvent(Event);
		}
	}
}

void UPlayMontageProStatics::HandleTimeDilation(IPlayMontageProInterface* Interface, const USkinnedMeshComponent* MeshComp,
	float& TimeDilation, TArray<FAnimNotifyProEvent>& Notifies)
{
	TRACE_CPUPROFILER_EVENT_SCOPE(UPlayMontageProStatics::HandleTimeDilation);
	
	const UWorld* World = MeshComp ? MeshComp->GetWorld() : nullptr;
	if (!World)
	{
		return;
	}
	
	const float NewTimeDilation = MeshComp->GetOwner()->CustomTimeDilation;
	if (!FMath::IsNearlyEqual(TimeDilation, NewTimeDilation))
	{
		// If time dilation has changed, we need to update the notifies
		for (FAnimNotifyProEvent& Notify : Notifies)
		{
			// Any elapsed time should be maintained, and only remaining time should be updated
			// Then we need to restart the timer based on the new time, without the already elapsed time
			// So that only the remaining time is affected by time dilation changes
			if (Notify.IsValid() && !Notify.bNotifySkipped && !Notify.bHasBroadcast && Notify.Timer.IsValid())
			{
				const float ElapsedTime = World->GetTimerManager().GetTimerElapsed(Notify.Timer);
				const float RemainingTime = Notify.Time - ElapsedTime;
				if (RemainingTime > 0.f)
				{
					// Restart the timer with the new time dilation
					Notify.Time = ElapsedTime + RemainingTime / NewTimeDilation;

					// Clear the previous delegate and bind a new one
					World->GetTimerManager().ClearTimer(Notify.Timer);
					Notify.TimerDelegate.Unbind();
					Notify.TimerDelegate = Interface->CreateTimerDelegate(Notify);
					World->GetTimerManager().SetTimer(Notify.Timer, Notify.TimerDelegate, Notify.Time, false);
				}
			}
		}
		TimeDilation = NewTimeDilation;
	}
}
