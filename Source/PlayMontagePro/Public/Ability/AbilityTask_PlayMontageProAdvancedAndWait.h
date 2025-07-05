// Copyright (c) Jared Taylor
#pragma once

#include "CoreMinimal.h"
#include "PlayMontageProInterface.h"
#include "PlayMontageProStatics.h"
#include "PlayMontageTypes.h"
#include "UObject/ObjectMacros.h"
#include "Animation/AnimInstance.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "Animation/AnimMontage.h"
#include "AbilityTask_PlayMontageProAdvancedAndWait.generated.h"

struct FMontageBlendSettings;
class UAnimMontage;
class UPMPGameplayAbility;
class UPMPAbilitySystemComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FMontageAdvancedWaitEventDelegate, FGameplayTag, EventTag, FGameplayEventData, EventData);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMontageAdvancedWaitNotifyDelegate, const FAnimNotifyProEvent&, Event);

/** Ability task to simply play a montage. Many games will want to make a modified version of this task that looks for game-specific events */
UCLASS()
class PLAYMONTAGEPRO_API UAbilityTask_PlayMontageProAdvancedAndWait : public UAbilityTask, public IPlayMontageProInterface
{
	GENERATED_BODY()
public:

	UPROPERTY(BlueprintAssignable)
	FMontageAdvancedWaitEventDelegate	OnCompleted;

	UPROPERTY(BlueprintAssignable)
	FMontageAdvancedWaitEventDelegate	OnBlendedIn;

	UPROPERTY(BlueprintAssignable)
	FMontageAdvancedWaitEventDelegate	OnBlendOut;

	UPROPERTY(BlueprintAssignable)
	FMontageAdvancedWaitEventDelegate	OnInterrupted;

	UPROPERTY(BlueprintAssignable)
	FMontageAdvancedWaitEventDelegate	OnCancelled;

	UPROPERTY(BlueprintAssignable)
	FMontageAdvancedWaitEventDelegate	OnEventReceived;

	UPROPERTY(BlueprintAssignable)
	FMontageAdvancedWaitNotifyDelegate	OnNotify;

	UPROPERTY(BlueprintAssignable)
	FMontageAdvancedWaitNotifyDelegate	OnNotifyStateBegin;

	UPROPERTY(BlueprintAssignable)
	FMontageAdvancedWaitNotifyDelegate	OnNotifyStateEnd;
	
	UFUNCTION()
	void OnMontageBlendedIn(UAnimMontage* Montage);

	UFUNCTION()
	void OnMontageBlendingOut(UAnimMontage* Montage, bool bInterrupted);

	/** Callback function for when the owning Gameplay Ability is cancelled */
	UFUNCTION()
	void OnGameplayAbilityCancelled();

	UFUNCTION()
	void OnMontageEnded(UAnimMontage* Montage, bool bInterrupted);

	/** 
	 * Start playing an animation montage on the avatar actor and wait for it to finish
	 * If StopWhenAbilityEnds is true, this montage will be aborted if the ability ends normally. It is always stopped when the ability is explicitly cancelled.
	 * On normal execution, OnBlendOut is called when the montage is blending out, and OnCompleted when it is completely done playing
	 * OnInterrupted is called if another montage overwrites this, and OnCancelled is called if the ability or task is cancelled
	 *
	 * @param OwningAbility
	 * @param TaskInstanceName Set to override the name of this task, for later querying
	 * @param EventTags Event tags to listen for. Any gameplay events matching these tags will activate the EventReceived callback. If empty, all events will trigger callback
	 * @param MontageToPlay The montage(s) to play on the character
	 * @param bDrivenMontagesMatchDriverDuration If true, all driven montages will run for the same duration as the driver montage
	 * @param Rate Change to play the montage faster or slower
	 * @param StartSection If not empty, named montage section to start from
	 * @param ProNotifyParams Parameters for Pro notifies, which trigger reliably unlike Epic's notify system.
	 * @param bOverrideBlendIn If true apply BlendInOverride settings instead of the settings assigned to the montage
	 * @param BlendInOverride Settings to use if bOverrideBlendIn is true
	 * @param bStopWhenAbilityEnds If true, this montage will be aborted if the ability ends normally. It is always stopped when the ability is explicitly cancelled
	 * @param AnimRootMotionTranslationScale Change to modify size of root motion or set to 0 to block it entirely
	 * @param StartTimeSeconds Starting time offset in montage, this will be overridden by StartSection if that is also set
	 * @param bAllowInterruptAfterBlendOut If true, you can receive OnInterrupted after an OnBlendOut started (otherwise OnInterrupted will not fire when interrupted, but you will not get OnComplete).
	 * @param OverrideBlendOutTimeOnCancelAbility If >= 0 it will override the blend out time when ability is cancelled.
	 * @param OverrideBlendOutTimeOnEndAbility If >= 0 it will override the blend out time when ability ends.
	 */
	UFUNCTION(BlueprintCallable, Category="Ability|Tasks", meta = (DisplayName="PlayMontageProAdvancedAndWait",
		HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "TRUE"))
	static UAbilityTask_PlayMontageProAdvancedAndWait* CreatePlayMontageProAdvancedAndWaitProxy(
		UPMPGameplayAbility* OwningAbility,
		FName TaskInstanceName,
		FGameplayTagContainer EventTags,
		FMontageToPlay MontageToPlay,
		bool bDrivenMontagesMatchDriverDuration = true,
		float Rate = 1.f,
		FName StartSection = NAME_None,
		FProNotifyParams ProNotifyParams = FProNotifyParams(),
		bool bOverrideBlendIn = false,
		FMontageBlendSettings BlendInOverride = FMontageBlendSettings(),
		bool bStopWhenAbilityEnds = true,
		float AnimRootMotionTranslationScale = 1.f,
		float StartTimeSeconds = 0.f,
		bool bAllowInterruptAfterBlendOut = false,
		float OverrideBlendOutTimeOnCancelAbility = -1.f,
		float OverrideBlendOutTimeOnEndAbility = -1.f);

	virtual void Activate() override;

	void PlayDrivenMontageForMesh(float Duration, const FDrivenMontagePair& Montage, bool bReplicate) const;
	
	UPMPAbilitySystemComponent* GetASC() const;
	
	/** Called when the ability is asked to cancel from an outside node. What this means depends on the individual task. By default, this does nothing other than ending the task. */
	virtual void ExternalCancel() override;

	virtual FString GetDebugString() const override;
	
public:
	// Begin IPlayMontageProInterface
	virtual void BroadcastNotifyEvent(FAnimNotifyProEvent& Event) override { UPlayMontageProStatics::BroadcastNotifyEvent(Event, this); }
	virtual void NotifyCallback(const FAnimNotifyProEvent& Event) override { OnNotify.Broadcast(Event); }
	virtual void NotifyBeginCallback(const FAnimNotifyProEvent& Event) override { OnNotifyStateBegin.Broadcast(Event); }
	virtual void NotifyEndCallback(const FAnimNotifyProEvent& Event) override { OnNotifyStateEnd.Broadcast(Event); }

	virtual UAnimMontage* GetMontage() const override final;
	virtual USkeletalMeshComponent* GetMesh() const override final;

	virtual FTimerDelegate CreateTimerDelegate(FAnimNotifyProEvent& Event) override { return FTimerDelegate::CreateUObject(this, &IPlayMontageProInterface::OnNotifyTimer, &Event); }
	// ~End IPlayMontageProInterface
	
protected:
	void OnGameplayEvent(FGameplayTag EventTag, const FGameplayEventData* Payload);
	
	UFUNCTION()
	void OnMontageSectionChanged(UAnimMontage* InMontage, FName SectionName, bool bLooped);
	
	UFUNCTION()
	void OnTickPose(USkinnedMeshComponent* SkinnedMeshComponent, float DeltaTime, bool NeedsValidRootMotion);

	virtual void OnDestroy(bool AbilityEnded) override;

	UFUNCTION(BlueprintCallable, Category="Ability|Tasks")
	void MontageJumpToSection(FName SectionName, bool bOnlyDriver = false);

	UFUNCTION(BlueprintCallable, Category="Ability|Tasks")
	void MontageSetNextSection(FName FromSection, FName ToSection, bool bOnlyDriver = false);
	
	/** Checks if the ability is playing a montage and stops that montage, returns true if a montage was stopped, false if not. */
	UFUNCTION(BlueprintCallable, Category="Ability|Tasks")
	bool StopPlayingMontage(float OverrideBlendOutTime = -1.f);

	UFUNCTION(BlueprintCallable, Category="Ability|Tasks")
	bool IsPlayingMontage() const;

	FOnMontageBlendedInEnded BlendedInDelegate;
	FOnMontageBlendingOutStarted BlendingOutDelegate;
	FOnMontageEnded MontageEndedDelegate;
	FDelegateHandle InterruptedHandle;

	UPROPERTY()
	FGameplayTagContainer EventTags;
	
	UPROPERTY()
	TObjectPtr<UAnimMontage> MontageToPlay;
	
	UPROPERTY()
	FDrivenMontages DrivenMontages;

	UPROPERTY()
	bool bDrivenMontagesMatchDriverDuration;

	UPROPERTY()
	bool bOverrideBlendIn;

	UPROPERTY()
	FMontageBlendSettings BlendInOverride;
	
	UPROPERTY()
	float Rate;

	UPROPERTY()
	FName StartSection;

	UPROPERTY()
	FProNotifyParams ProNotifyParams;

	UPROPERTY()
	float AnimRootMotionTranslationScale;

	UPROPERTY()
	float StartTimeSeconds;

	UPROPERTY()
	bool bStopWhenAbilityEnds;

	UPROPERTY()
	bool bAllowInterruptAfterBlendOut;

	UPROPERTY()
	float OverrideBlendOutTimeOnCancelAbility;

	UPROPERTY()
	float OverrideBlendOutTimeOnEndAbility;

	UPROPERTY()
	uint32 NotifyId = 0;
	
	UPROPERTY()
	TArray<FAnimNotifyProEvent> Notifies;
	
	FDelegateHandle TickPoseHandle;

	FDelegateHandle EventHandle;
	
	float TimeDilation = 1.f;
};
