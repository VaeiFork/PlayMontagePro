// Copyright (c) Jared Taylor

#include "PlayMontageProCallbackProxy.h"

#include "PlayMontageProStatics.h"
#include "Animation/AnimMontage.h"
#include "Components/SkeletalMeshComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(PlayMontageProCallbackProxy)

//////////////////////////////////////////////////////////////////////////
// UPlayMontageProCallbackProxy

UPlayMontageProCallbackProxy::UPlayMontageProCallbackProxy(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, MontageInstanceID(INDEX_NONE)
	, bInterruptedCalledBeforeBlendingOut(false)
{
}

UPlayMontageProCallbackProxy* UPlayMontageProCallbackProxy::CreateProxyObjectForPlayMontagePro(
	class USkeletalMeshComponent* InSkeletalMeshComponent,
	class UAnimMontage* MontageToPlay,
	float PlayRate,
	float StartingPosition,
	FName StartingSection,
	bool bTriggerNotifiesBeforeStartTime,
	bool bShouldStopAllMontages)
{
	UPlayMontageProCallbackProxy* Proxy = NewObject<UPlayMontageProCallbackProxy>();
	Proxy->SetFlags(RF_StrongRefOnFrame);
	Proxy->PlayMontagePro(InSkeletalMeshComponent, MontageToPlay, PlayRate, StartingPosition, StartingSection, bTriggerNotifiesBeforeStartTime, bShouldStopAllMontages);
	return Proxy;
}

bool UPlayMontageProCallbackProxy::PlayMontagePro(class USkeletalMeshComponent* InSkeletalMeshComponent, 
	class UAnimMontage* MontageToPlay, 
	float PlayRate, 
	float StartingPosition, 
	FName StartingSection,
	bool bTriggerNotifiesBeforeStartTime,
	bool bShouldStopAllMontages)
{
	MeshComp = InSkeletalMeshComponent;
	Montage = MontageToPlay;
	
	bool bPlayedSuccessfully = false;
	if (InSkeletalMeshComponent)
	{
		if (UAnimInstance* AnimInstance = InSkeletalMeshComponent->GetAnimInstance())
		{
			const float MontageLength = AnimInstance->Montage_Play(MontageToPlay, PlayRate, EMontagePlayReturnType::MontageLength, StartingPosition, bShouldStopAllMontages);
			bPlayedSuccessfully = MontageLength > 0.f;

			if (bPlayedSuccessfully)
			{
				// Gather notifies from montage
				UPlayMontageProStatics::GatherNotifies(MontageToPlay, NotifyId, Notifies);

				// Trigger notifies before start time and remove them, if we want to trigger them before the start time
				UPlayMontageProStatics::TriggerHistoricNotifies(Notifies, StartingPosition, bTriggerNotifiesBeforeStartTime, this);

				// Create timer delegates for notifies
				UPlayMontageProStatics::SetupNotifyTimers(this, InSkeletalMeshComponent->GetWorld(), Notifies);
				
				// Engine default handling
				
				AnimInstancePtr = AnimInstance;
				if (const FAnimMontageInstance* MontageInstance = AnimInstance->GetActiveInstanceForMontage(MontageToPlay))
				{
					MontageInstanceID = MontageInstance->GetInstanceID();
				}

				if (StartingSection != NAME_None)
				{
					AnimInstance->Montage_JumpToSection(StartingSection, MontageToPlay);
				}

				BlendingOutDelegate.BindUObject(this, &ThisClass::OnMontageBlendingOut);
				AnimInstance->Montage_SetBlendingOutDelegate(BlendingOutDelegate, MontageToPlay);

				MontageEndedDelegate.BindUObject(this, &ThisClass::OnMontageEnded);
				AnimInstance->Montage_SetEndDelegate(MontageEndedDelegate, MontageToPlay);
			}
		}
	}

	if (!bPlayedSuccessfully)
	{
		OnInterrupted.Broadcast(NAME_None);
	}

	return bPlayedSuccessfully;
}

bool UPlayMontageProCallbackProxy::IsNotifyValid(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPointNotifyPayload) const
{
	return ((MontageInstanceID != INDEX_NONE) && (BranchingPointNotifyPayload.MontageInstanceID == MontageInstanceID));
}

void UPlayMontageProCallbackProxy::OnMontageBlendingOut(UAnimMontage* InMontage, bool bInterrupted)
{
	if (bInterrupted)
	{
		OnInterrupted.Broadcast(NAME_None);
		bInterruptedCalledBeforeBlendingOut = true;
		UPlayMontageProStatics::EnsureBroadcastNotifyEvents(EAnimNotifyProEventType::OnInterrupted, Notifies, this);
	}
	else
	{
		OnBlendOut.Broadcast(NAME_None);
		UPlayMontageProStatics::EnsureBroadcastNotifyEvents(EAnimNotifyProEventType::BlendOut, Notifies, this);
	}
}

void UPlayMontageProCallbackProxy::OnMontageEnded(UAnimMontage* InMontage, bool bInterrupted)
{
	if (!bInterrupted)
	{
		OnCompleted.Broadcast(NAME_None);
		UPlayMontageProStatics::EnsureBroadcastNotifyEvents(EAnimNotifyProEventType::OnCompleted, Notifies, this);
	}
	else if (!bInterruptedCalledBeforeBlendingOut)
	{
		OnInterrupted.Broadcast(NAME_None);
		UPlayMontageProStatics::EnsureBroadcastNotifyEvents(EAnimNotifyProEventType::OnInterrupted, Notifies, this);
	}
}
