// Copyright (c) Jared Taylor

#pragma once

#include "CoreMinimal.h"
#include "PlayMontageProInterface.h"
#include "PlayMontageProStatics.h"
#include "PlayMontageTypes.h"
#include "UObject/ObjectMacros.h"
#include "UObject/Object.h"
#include "PlayMontageProCallbackProxy.generated.h"

class UAnimNotifyStatePro;
class UAnimNotifyPro;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMontagePlayDelegate, FName, NotifyName);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMontagePlayNotifyDelegate, const FAnimNotifyProEvent&, Event);

UCLASS()
class PLAYMONTAGEPRO_API UPlayMontageProCallbackProxy : public UObject, public IPlayMontageProInterface
{
	GENERATED_UCLASS_BODY()

	// Called when Montage finished playing and wasn't interrupted
	UPROPERTY(BlueprintAssignable)
	FOnMontagePlayDelegate OnCompleted;

	// Called when Montage starts blending out and is not interrupted
	UPROPERTY(BlueprintAssignable)
	FOnMontagePlayDelegate OnBlendOut;

	// Called when Montage has been interrupted (or failed to play)
	UPROPERTY(BlueprintAssignable)
	FOnMontagePlayDelegate OnInterrupted;

	UPROPERTY(BlueprintAssignable)
	FOnMontagePlayNotifyDelegate OnNotify;
	
	UPROPERTY(BlueprintAssignable)
	FOnMontagePlayNotifyDelegate OnNotifyStateBegin;

	UPROPERTY(BlueprintAssignable)
	FOnMontagePlayNotifyDelegate OnNotifyStateEnd;

	UPROPERTY()
	uint32 NotifyId = 0;

	UPROPERTY()
	TWeakObjectPtr<UAnimMontage> Montage;

	UPROPERTY()
	TWeakObjectPtr<USkeletalMeshComponent> MeshComp;
	
	UPROPERTY()
	TArray<FAnimNotifyProEvent> Notifies;
	
	// Called to perform the query internally
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true"))
	static UPlayMontageProCallbackProxy* CreateProxyObjectForPlayMontagePro(
		class USkeletalMeshComponent* InSkeletalMeshComponent, 
		class UAnimMontage* MontageToPlay, 
		float PlayRate = 1.f, 
		float StartingPosition = 0.f, 
		FName StartingSection = NAME_None,
		bool bTriggerNotifiesBeforeStartTime = false,
		bool bShouldStopAllMontages = true);

public:
	// Begin IPlayMontageProInterface
	virtual void BroadcastNotifyEvent(FAnimNotifyProEvent& Event) override { UPlayMontageProStatics::BroadcastNotifyEvent(Event, this); }
	virtual void NotifyCallback(const FAnimNotifyProEvent& Event) override { OnNotify.Broadcast(Event); }
	virtual void NotifyBeginCallback(const FAnimNotifyProEvent& Event) override { OnNotifyStateBegin.Broadcast(Event); }
	virtual void NotifyEndCallback(const FAnimNotifyProEvent& Event) override { OnNotifyStateEnd.Broadcast(Event); }

	virtual UAnimMontage* GetMontage() const override final { return Montage.IsValid() ? Montage.Get() : nullptr; }
	virtual USkeletalMeshComponent* GetMesh() const override final { return MeshComp.IsValid() ? MeshComp.Get() : nullptr; }

	virtual FTimerDelegate CreateTimerDelegate(FAnimNotifyProEvent& Event) override { return FTimerDelegate::CreateUObject(this, &IPlayMontageProInterface::OnNotifyTimer, &Event); }
	// ~End IPlayMontageProInterface
	
protected:
	UFUNCTION()
	void OnMontageBlendingOut(UAnimMontage* InMontage, bool bInterrupted);

	UFUNCTION()
	void OnMontageEnded(UAnimMontage* InMontage, bool bInterrupted);

private:
	TWeakObjectPtr<UAnimInstance> AnimInstancePtr;
	int32 MontageInstanceID;
	uint32 bInterruptedCalledBeforeBlendingOut : 1;

	bool IsNotifyValid(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPointNotifyPayload) const;

	FOnMontageBlendingOutStarted BlendingOutDelegate;
	FOnMontageEnded MontageEndedDelegate;

protected:
	// Attempts to play a montage with the specified settings. Returns whether it started or not.
	bool PlayMontagePro(
		class USkeletalMeshComponent* InSkeletalMeshComponent,
		class UAnimMontage* MontageToPlay,
		float PlayRate = 1.f,
		float StartingPosition = 0.f,
		FName StartingSection = NAME_None,
		bool bTriggerNotifiesBeforeStartTime = false,
		bool bShouldStopAllMontages = true);
};
