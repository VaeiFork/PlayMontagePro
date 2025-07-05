// Copyright (c) Jared Taylor

#include "Ability/AbilityTask_PlayMontageProAdvancedAndWait.h"
#include "Animation/AnimMontage.h"
#include "GameFramework/Character.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemLog.h"
#include "AbilitySystemGlobals.h"
#include "Ability/PMPAbilitySystemComponent.h"
#include "Ability/PMPGameplayAbility.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(AbilityTask_PlayMontageProAdvancedAndWait)

static bool GUseAggressivePlayMontageProAdvancedAndWaitEndTask = true;
static FAutoConsoleVariableRef CVarAggressivePlayMontageProAdvancedAndWaitEndTask(TEXT("AbilitySystem.PlayMontageProAdvanced.AggressiveEndTask"), GUseAggressivePlayMontageProAdvancedAndWaitEndTask, TEXT("This should be set to true in order to avoid multiple callbacks off an AbilityTask_PlayMontageProAdvancedAndWait node"));

static bool GPlayMontageProAdvancedAndWaitFireInterruptOnAnimEndInterrupt = true;
static FAutoConsoleVariableRef CVarPlayMontageProAdvancedAndWaitFireInterruptOnAnimEndInterrupt(TEXT("AbilitySystem.PlayMontageProAdvanced.FireInterruptOnAnimEndInterrupt"), GPlayMontageProAdvancedAndWaitFireInterruptOnAnimEndInterrupt, TEXT("This is a fix that will cause AbilityTask_PlayMontageProAdvancedAndWait to fire its Interrupt event if the underlying AnimInstance ends in an interrupted"));

void UAbilityTask_PlayMontageProAdvancedAndWait::OnMontageBlendingOut(UAnimMontage* Montage, bool bInterrupted)
{
	const bool bPlayingThisMontage = (Montage == MontageToPlay) && Ability && Ability->GetCurrentMontage() == MontageToPlay;
	if (bPlayingThisMontage)
	{
		// Reset AnimRootMotionTranslationScale
		ACharacter* Character = Cast<ACharacter>(GetAvatarActor());
		if (Character && (Character->GetLocalRole() == ROLE_Authority || (Character->GetLocalRole() == ROLE_AutonomousProxy &&
			Ability->GetNetExecutionPolicy() == EGameplayAbilityNetExecutionPolicy::LocalPredicted)))
		{
			Character->SetAnimRootMotionTranslationScale(1.f);
		}
	}

	if (bPlayingThisMontage && (bInterrupted || !bAllowInterruptAfterBlendOut))
	{
		if (UAbilitySystemComponent* ASC = AbilitySystemComponent.Get())
		{
			ASC->ClearAnimatingAbility(Ability);
		}
	}

	if (ShouldBroadcastAbilityTaskDelegates())
	{
		if (bInterrupted)
		{
			UPlayMontageProStatics::EnsureBroadcastNotifyEvents(EAnimNotifyProEventType::OnInterrupted, Notifies, this);
			
            bAllowInterruptAfterBlendOut = false;
			OnInterrupted.Broadcast(FGameplayTag(), FGameplayEventData());

			if (GUseAggressivePlayMontageProAdvancedAndWaitEndTask)
			{
				EndTask();
			}
		}
		else
		{
			UPlayMontageProStatics::EnsureBroadcastNotifyEvents(EAnimNotifyProEventType::BlendOut, Notifies, this);
			
			OnBlendOut.Broadcast(FGameplayTag(), FGameplayEventData());
		}
	}
}

void UAbilityTask_PlayMontageProAdvancedAndWait::OnMontageBlendedIn(UAnimMontage* Montage)
{
	if (ShouldBroadcastAbilityTaskDelegates())
	{
		OnBlendedIn.Broadcast(FGameplayTag(), FGameplayEventData());
	}
}

void UAbilityTask_PlayMontageProAdvancedAndWait::OnGameplayAbilityCancelled()
{
	if (StopPlayingMontage(OverrideBlendOutTimeOnCancelAbility) || bAllowInterruptAfterBlendOut)
	{
		// Let the BP handle the interrupt as well
		if (ShouldBroadcastAbilityTaskDelegates())
		{
			UPlayMontageProStatics::EnsureBroadcastNotifyEvents(EAnimNotifyProEventType::OnInterrupted, Notifies, this);
			bAllowInterruptAfterBlendOut = false;
			OnInterrupted.Broadcast(FGameplayTag(), FGameplayEventData());
		}
	}

	if (GUseAggressivePlayMontageProAdvancedAndWaitEndTask)
	{
		EndTask();
	}
}

void UAbilityTask_PlayMontageProAdvancedAndWait::OnMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	if (!bInterrupted)
	{
		if (ShouldBroadcastAbilityTaskDelegates())
		{
			UPlayMontageProStatics::EnsureBroadcastNotifyEvents(EAnimNotifyProEventType::OnCompleted, Notifies, this);
			OnCompleted.Broadcast(FGameplayTag(), FGameplayEventData());
		}
	}
	else if (bAllowInterruptAfterBlendOut && GPlayMontageProAdvancedAndWaitFireInterruptOnAnimEndInterrupt)
	{
		if (ShouldBroadcastAbilityTaskDelegates())
		{
			UPlayMontageProStatics::EnsureBroadcastNotifyEvents(EAnimNotifyProEventType::OnInterrupted, Notifies, this);
			OnInterrupted.Broadcast(FGameplayTag(), FGameplayEventData());
		}
	}

	EndTask();
}

UAbilityTask_PlayMontageProAdvancedAndWait* UAbilityTask_PlayMontageProAdvancedAndWait::
CreatePlayMontageProAdvancedAndWaitProxy(UPMPGameplayAbility* OwningAbility, FName TaskInstanceName,
	FGameplayTagContainer EventTags, FMontageToPlay MontageToPlay, bool bDrivenMontagesMatchDriverDuration, float Rate,
	FName StartSection, FProNotifyParams ProNotifyParams, bool bOverrideBlendIn, FMontageBlendSettings BlendInOverride,
	bool bStopWhenAbilityEnds, float AnimRootMotionTranslationScale, float StartTimeSeconds,
	bool bAllowInterruptAfterBlendOut, float OverrideBlendOutTimeOnCancelAbility,
	float OverrideBlendOutTimeOnEndAbility)
{
	UAbilitySystemGlobals::NonShipping_ApplyGlobalAbilityScaler_Rate(Rate);

#if !UE_BUILD_SHIPPING
	if (!ensure(MontageToPlay.IsValid()))
	{
		FMessageLog("PIE").Error(FText::Format(NSLOCTEXT("PlayMontageProAdvanced", "InvalidMontage",
			"UAbilityTask_PlayMontageProAdvancedAndWait: MontageToPlay is invalid for task {0} on ability {1}"),
			FText::FromName(TaskInstanceName), FText::FromString(GetNameSafe(OwningAbility))));
	}
#endif
	
	UAbilityTask_PlayMontageProAdvancedAndWait* MyObj = NewAbilityTask<UAbilityTask_PlayMontageProAdvancedAndWait>(OwningAbility, TaskInstanceName);
	MyObj->EventTags = EventTags;
	MyObj->MontageToPlay = MontageToPlay.DriverMontage;
	MyObj->DrivenMontages = MontageToPlay.DrivenMontages;
	MyObj->bDrivenMontagesMatchDriverDuration = bDrivenMontagesMatchDriverDuration;
	MyObj->Rate = Rate;
	MyObj->StartSection = StartSection;
	MyObj->ProNotifyParams = ProNotifyParams;
	MyObj->bOverrideBlendIn = bOverrideBlendIn;
	MyObj->BlendInOverride = BlendInOverride;
	MyObj->AnimRootMotionTranslationScale = AnimRootMotionTranslationScale;
	MyObj->bStopWhenAbilityEnds = bStopWhenAbilityEnds;
	MyObj->bAllowInterruptAfterBlendOut = bAllowInterruptAfterBlendOut;
	MyObj->StartTimeSeconds = StartTimeSeconds;
	MyObj->OverrideBlendOutTimeOnCancelAbility = OverrideBlendOutTimeOnCancelAbility;
	MyObj->OverrideBlendOutTimeOnEndAbility = OverrideBlendOutTimeOnEndAbility;
	
	return MyObj;
}

void UAbilityTask_PlayMontageProAdvancedAndWait::Activate()
{
	if (Ability == nullptr)
	{
		return;
	}

	bool bPlayedMontage = false;

	if (UPMPAbilitySystemComponent* ASC = GetASC())
	{
		const FGameplayAbilityActorInfo* ActorInfo = Ability->GetCurrentActorInfo();
		UAnimInstance* AnimInstance = ActorInfo->GetAnimInstance();
		if (AnimInstance != nullptr)
		{
			// Bind to event callback
			EventHandle = ASC->AddGameplayEventTagContainerDelegate(EventTags,
				FGameplayEventTagMulticastDelegate::FDelegate::CreateUObject(this, &ThisClass::OnGameplayEvent));

			// Play Driver Montage
			const float Duration = ASC->PlayMontageForMesh(Ability, ActorInfo->SkeletalMeshComponent.Get(),
				Ability->GetCurrentActivationInfo(), MontageToPlay, Rate, bOverrideBlendIn, BlendInOverride,
				StartSection, StartTimeSeconds, true);

			bPlayedMontage = Duration > 0.f;

			// Play Driven Montages
			if (bPlayedMontage)
			{
				for (const auto& Montage : DrivenMontages.DrivenMontages)
				{
					PlayDrivenMontageForMesh(Duration, Montage, true);
				}

				for (const auto& Montage : DrivenMontages.LocalDrivenMontages)
				{
					PlayDrivenMontageForMesh(Duration, Montage, false);
				}
				
				// Playing a montage could potentially fire off a callback into game code which could kill this ability! Early out if we are  pending kill.
				if (ShouldBroadcastAbilityTaskDelegates() == false)
				{
					return;
				}

				InterruptedHandle = Ability->OnGameplayAbilityCancelled.AddUObject(this, &ThisClass::OnGameplayAbilityCancelled);

				BlendedInDelegate.BindUObject(this, &ThisClass::OnMontageBlendedIn);
				AnimInstance->Montage_SetBlendedInDelegate(BlendedInDelegate, MontageToPlay);

				BlendingOutDelegate.BindUObject(this, &ThisClass::OnMontageBlendingOut);
				AnimInstance->Montage_SetBlendingOutDelegate(BlendingOutDelegate, MontageToPlay);

				MontageEndedDelegate.BindUObject(this, &ThisClass::OnMontageEnded);
				AnimInstance->Montage_SetEndDelegate(MontageEndedDelegate, MontageToPlay);

				ACharacter* Character = Cast<ACharacter>(GetAvatarActor());
				if (Character && (Character->GetLocalRole() == ROLE_Authority ||
								  (Character->GetLocalRole() == ROLE_AutonomousProxy && Ability->GetNetExecutionPolicy() == EGameplayAbilityNetExecutionPolicy::LocalPredicted)))
				{
					Character->SetAnimRootMotionTranslationScale(AnimRootMotionTranslationScale);
				}

				if (ProNotifyParams.bEnableProNotifies)
				{
					// -- PlayMontagePro --
				
					// Use the mesh comp's OnTickPose to detect time dilation changes
					if (ProNotifyParams.bEnableCustomTimeDilation && GetMesh() && ActorInfo->AvatarActor.IsValid())
					{
						TimeDilation = ActorInfo->AvatarActor->CustomTimeDilation;
						TickPoseHandle = GetMesh()->OnTickPose.AddUObject(this, &ThisClass::OnTickPose);
					}
					else
					{
						TimeDilation = 1.f;
					}

					if (StartSection != NAME_None)
					{
						// PlayMontagePro needs to update StartingPosition to account for the section jump
						const float NewPosition = AnimInstance->Montage_GetPosition(MontageToPlay);
						StartTimeSeconds += (NewPosition - StartTimeSeconds);
					}

					// Handle section changes
					AnimInstance->OnMontageSectionChanged.AddDynamic(this, &ThisClass::OnMontageSectionChanged);

					// Gather notifies from montage
					const FName Section = AnimInstance->Montage_GetCurrentSection(MontageToPlay);
					UPlayMontageProStatics::GatherNotifies(MontageToPlay, NotifyId, Notifies, Section, StartTimeSeconds, TimeDilation);

					// Trigger notifies before start time and remove them, if we want to trigger them before the start time
					UPlayMontageProStatics::HandleHistoricNotifies(Notifies, ProNotifyParams.bTriggerNotifiesBeforeStartTime, this);

					// Create timer delegates for notifies
					UPlayMontageProStatics::SetupNotifyTimers(this, GetWorld(), Notifies);
				}
			}
		}
		else
		{
			ABILITY_LOG(Warning, TEXT("UAbilityTask_PlayMontageProAdvancedAndWait call to PlayMontagePro failed!"));
		}
	}
	else
	{
		ABILITY_LOG(Warning, TEXT("UAbilityTask_PlayMontageProAdvancedAndWait called on invalid AbilitySystemComponent"));
	}

	if (!bPlayedMontage)
	{
		ABILITY_LOG(Warning, TEXT("UAbilityTask_PlayMontageProAdvancedAndWait called in Ability %s failed to play montage %s; Task Instance Name %s."), *Ability->GetName(), *GetNameSafe(MontageToPlay),*InstanceName.ToString());
		if (ShouldBroadcastAbilityTaskDelegates())
		{
			UPlayMontageProStatics::EnsureBroadcastNotifyEvents(EAnimNotifyProEventType::OnCancelled, Notifies, this);
			OnCancelled.Broadcast(FGameplayTag(), FGameplayEventData());
		}
	}

	SetWaitingOnAvatar();
}

void UAbilityTask_PlayMontageProAdvancedAndWait::PlayDrivenMontageForMesh(float Duration,
	const FDrivenMontagePair& Montage, bool bReplicate) const
{
	const float ScaledRate = !bDrivenMontagesMatchDriverDuration ? Rate :
		Rate * UPlayMontageProStatics::GetMontagePlayRateScaledByDuration(Montage.Montage, Duration);
	
	GetASC()->PlayMontageForMesh(Ability, Montage.Mesh, Ability->GetCurrentActivationInfo(), Montage.Montage,
		ScaledRate, bOverrideBlendIn, BlendInOverride, StartSection, StartTimeSeconds, bReplicate);
}

UPMPAbilitySystemComponent* UAbilityTask_PlayMontageProAdvancedAndWait::GetASC() const
{
	return AbilitySystemComponent.IsValid() ? Cast<UPMPAbilitySystemComponent>(AbilitySystemComponent.Get()) : nullptr;
}

void UAbilityTask_PlayMontageProAdvancedAndWait::ExternalCancel()
{
	if (ShouldBroadcastAbilityTaskDelegates())
	{
		UPlayMontageProStatics::EnsureBroadcastNotifyEvents(EAnimNotifyProEventType::OnCancelled, Notifies, this);
		OnCancelled.Broadcast(FGameplayTag(), FGameplayEventData());
	}
	Super::ExternalCancel();
}

void UAbilityTask_PlayMontageProAdvancedAndWait::OnGameplayEvent(FGameplayTag EventTag,
	const FGameplayEventData* Payload)
{
	if (ShouldBroadcastAbilityTaskDelegates())
	{
		FGameplayEventData TempData = *Payload;
		TempData.EventTag = EventTag;

		OnEventReceived.Broadcast(EventTag, TempData);
	}
}

void UAbilityTask_PlayMontageProAdvancedAndWait::OnMontageSectionChanged(UAnimMontage* InMontage, FName SectionName,
	bool bLooped)
{
	if (!ShouldBroadcastAbilityTaskDelegates() || !IsValid(MontageToPlay) || InMontage != MontageToPlay)
	{
		return;
	}

	const FGameplayAbilityActorInfo* ActorInfo = Ability->GetCurrentActorInfo();
	const UAnimInstance* AnimInstance = ActorInfo ? ActorInfo->GetAnimInstance() : nullptr;
	
	if (!ActorInfo || !AnimInstance)
	{
		return;
	}

	const float StartTime = AnimInstance->Montage_GetPosition(InMontage);

	// End previous notify timers
	UPlayMontageProStatics::ClearNotifyTimers(GetWorld(), Notifies);

	// Gather notifies from montage
	UPlayMontageProStatics::GatherNotifies(InMontage, NotifyId, Notifies, SectionName, StartTime, TimeDilation);

	// Create timer delegates for notifies
	UPlayMontageProStatics::SetupNotifyTimers(this, GetWorld(), Notifies);
}

void UAbilityTask_PlayMontageProAdvancedAndWait::OnTickPose(USkinnedMeshComponent* SkinnedMeshComponent, float DeltaTime,
	bool NeedsValidRootMotion)
{
	UPlayMontageProStatics::HandleTimeDilation(this, SkinnedMeshComponent, TimeDilation, Notifies);
}

void UAbilityTask_PlayMontageProAdvancedAndWait::OnDestroy(bool AbilityEnded)
{
	if (TickPoseHandle.IsValid() && GetMesh())
	{
		if (TickPoseHandle.IsValid() && GetMesh()->OnTickPose.IsBoundToObject(this))
		{
			GetMesh()->OnTickPose.Remove(TickPoseHandle);
		}
	}
	
	// Note: Clearing montage end delegate isn't necessary since its not a multicast and will be cleared when the next montage plays.
	// (If we are destroyed, it will detect this and not do anything)

	// This delegate, however, should be cleared as it is a multicast
	if (Ability)
	{
		Ability->OnGameplayAbilityCancelled.Remove(InterruptedHandle);
		if (AbilityEnded && bStopWhenAbilityEnds)
		{
			StopPlayingMontage(OverrideBlendOutTimeOnEndAbility);
		}
	}

	if (UAbilitySystemComponent* ASC = GetASC())
	{
		ASC->RemoveGameplayEventTagContainerDelegate(EventTags, EventHandle);
	}

	Super::OnDestroy(AbilityEnded);

}

void UAbilityTask_PlayMontageProAdvancedAndWait::MontageJumpToSection(FName SectionName, bool bOnlyDriver)
{
	if (Ability == nullptr)
	{
		return;
	}

	const FGameplayAbilityActorInfo* ActorInfo = Ability->GetCurrentActorInfo();
	if (ActorInfo == nullptr)
	{
		return;
	}

	const UAnimInstance* AnimInstance = ActorInfo->GetAnimInstance();
	if (AnimInstance == nullptr)
	{
		return;
	}

	UPMPAbilitySystemComponent* ASC = GetASC();
	USkeletalMeshComponent* Mesh = ActorInfo && ActorInfo->SkeletalMeshComponent.IsValid() ? ActorInfo->SkeletalMeshComponent.Get() : nullptr;

	if (ASC && Ability && Mesh)
	{
		if (ASC->GetAnimatingAbilityFromMesh(Mesh) == Ability && ASC->GetCurrentMontageForMesh(Mesh) == MontageToPlay)
		{
			// Driver Montage
			ASC->CurrentMontageJumpToSectionForMesh(Mesh, SectionName);

			if (!bOnlyDriver)
			{
				// Driven Montages
				for (const auto& Montage : DrivenMontages.DrivenMontages)
				{
					ASC->CurrentMontageJumpToSectionForMesh(Montage.Mesh, SectionName);
				}

				// Local Driven Montages
				for (const auto& Montage : DrivenMontages.LocalDrivenMontages)
				{
					ASC->CurrentMontageJumpToSectionForMesh(Montage.Mesh, SectionName);
				}
			}
		}
	}
}

void UAbilityTask_PlayMontageProAdvancedAndWait::MontageSetNextSection(FName FromSection, FName ToSection,
	bool bOnlyDriver)
{
	if (Ability == nullptr)
	{
		return;
	}

	const FGameplayAbilityActorInfo* ActorInfo = Ability->GetCurrentActorInfo();
	if (ActorInfo == nullptr)
	{
		return;
	}

	const UAnimInstance* AnimInstance = ActorInfo->GetAnimInstance();
	if (AnimInstance == nullptr)
	{
		return;
	}

	UPMPAbilitySystemComponent* ASC = GetASC();
	USkeletalMeshComponent* Mesh = ActorInfo && ActorInfo->SkeletalMeshComponent.IsValid() ? ActorInfo->SkeletalMeshComponent.Get() : nullptr;

	if (ASC && Ability && Mesh)
	{
		if (ASC->GetAnimatingAbilityFromMesh(Mesh) == Ability && ASC->GetCurrentMontageForMesh(Mesh) == MontageToPlay)
		{
			// Driver Montage
			ASC->CurrentMontageSetNextSectionNameForMesh(Mesh, FromSection, ToSection);

			if (!bOnlyDriver)
			{
				// Driven Montages
				for (const auto& Montage : DrivenMontages.DrivenMontages)
				{
					ASC->CurrentMontageSetNextSectionNameForMesh(Montage.Mesh, FromSection, ToSection);
				}

				// Local Driven Montages
				for (const auto& Montage : DrivenMontages.LocalDrivenMontages)
				{
					ASC->CurrentMontageSetNextSectionNameForMesh(Montage.Mesh, FromSection, ToSection);
				}
			}
		}
	}
}

bool UAbilityTask_PlayMontageProAdvancedAndWait::StopPlayingMontage(float OverrideBlendOutTime)
{
	if (Ability == nullptr)
	{
		return false;
	}

	const FGameplayAbilityActorInfo* ActorInfo = Ability->GetCurrentActorInfo();
	if (ActorInfo == nullptr)
	{
		return false;
	}

	const UAnimInstance* AnimInstance = ActorInfo->GetAnimInstance();
	if (AnimInstance == nullptr)
	{
		return false;
	}

	// Check if the montage is still playing
	// The ability would have been interrupted, in which case we should automatically stop the montage
	UPMPAbilitySystemComponent* ASC = GetASC();
	USkeletalMeshComponent* Mesh = ActorInfo && ActorInfo->SkeletalMeshComponent.IsValid() ? ActorInfo->SkeletalMeshComponent.Get() : nullptr;
	
	if (ASC && Ability && Mesh)
	{
		if (ASC->GetAnimatingAbilityFromMesh(Mesh) == Ability && ASC->GetCurrentMontageForMesh(Mesh) == MontageToPlay)
		{
			// Unbind delegates so they don't get called as well
			FAnimMontageInstance* MontageInstance = AnimInstance->GetActiveInstanceForMontage(MontageToPlay);
			if (MontageInstance)
			{
				MontageInstance->OnMontageBlendedInEnded.Unbind();
				MontageInstance->OnMontageBlendingOutStarted.Unbind();
				MontageInstance->OnMontageEnded.Unbind();
			}

			// Driver Montage
			ASC->CurrentMontageStopForMesh(Mesh, OverrideBlendOutTime);
			
			// Driven Montages
			for (const auto& Montage : DrivenMontages.DrivenMontages)
			{
				ASC->CurrentMontageStopForMesh(Montage.Mesh, OverrideBlendOutTime);
			}

			// Local Driven Montages
			for (const auto& Montage : DrivenMontages.LocalDrivenMontages)
			{
				ASC->CurrentMontageStopForMesh(Montage.Mesh, OverrideBlendOutTime);
			}
			
			return true;
		}
	}

	return false;
}

bool UAbilityTask_PlayMontageProAdvancedAndWait::IsPlayingMontage() const
{
	if (Ability == nullptr)
	{
		return false;
	}

	const FGameplayAbilityActorInfo* ActorInfo = Ability->GetCurrentActorInfo();
	if (ActorInfo == nullptr)
	{
		return false;
	}

	const UAnimInstance* AnimInstance = ActorInfo->GetAnimInstance();
	if (AnimInstance == nullptr)
	{
		return false;
	}

	// Check if the montage is still playing
	// The ability would have been interrupted, in which case we should automatically stop the montage
	UPMPAbilitySystemComponent* ASC = GetASC();
	if (!GetASC())
	{
		return false;
	}

	USkeletalMeshComponent* Mesh = ActorInfo && ActorInfo->SkeletalMeshComponent.IsValid() ? ActorInfo->SkeletalMeshComponent.Get() : nullptr;
	
	if (ASC && Ability && Mesh)
	{
		if (ASC->GetAnimatingAbilityFromMesh(Mesh) == Ability && ASC->GetCurrentMontageForMesh(Mesh) == MontageToPlay)
		{
			return true;
		}
	}

	return false;
}

FString UAbilityTask_PlayMontageProAdvancedAndWait::GetDebugString() const
{
	const UAnimMontage* PlayingMontage = nullptr;
	if (Ability)
	{
		const FGameplayAbilityActorInfo* ActorInfo = Ability->GetCurrentActorInfo();
		const UAnimInstance* AnimInstance = ActorInfo->GetAnimInstance();

		if (AnimInstance != nullptr)
		{
			PlayingMontage = AnimInstance->Montage_IsActive(MontageToPlay) ? ToRawPtr(MontageToPlay) : AnimInstance->GetCurrentActiveMontage();
		}
	}

	return FString::Printf(TEXT("PlayMontageProAdvancedAndWait. MontageToPlay: %s  (Currently Playing): %s"), *GetNameSafe(MontageToPlay), *GetNameSafe(PlayingMontage));
}

