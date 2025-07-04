// Copyright (c) Jared Taylor

#pragma once

#include "CoreMinimal.h"
#include "PlayMontageTypes.generated.h"

class UAnimNotifyStatePro;
class UAnimNotifyPro;

UENUM(BlueprintType)
enum class EAnimNotifyLegacyType : uint8
{
	Legacy			UMETA(ToolTip="Legacy behavior, notify will be triggered on simulated proxies no different to the old system"),
	Disabled		UMETA(ToolTip="Notify will not be triggered on simulated proxies, only on authority and local clients"),
};

UENUM(BlueprintType, meta = (Bitflags, UseEnumValuesAsMaskValuesInEditor="true"))
enum class EAnimNotifyProEventType : uint8
{
	None			= 0	UMETA(Hidden),
	OnCompleted		= 1 << 0,
	BlendOut		= 1 << 1,
	OnInterrupted	= 1 << 2,
	OnCancelled		= 1 << 3,
};
ENUM_CLASS_FLAGS(EAnimNotifyProEventType)

enum class EAnimNotifyProType : uint8
{
	Notify,
	NotifyStateBegin,
	NotifyStateEnd,
};

USTRUCT()
struct FAnimNotifyProEvent
{
	GENERATED_BODY()
	
	FAnimNotifyProEvent(uint32 InNotifyId = 0, int32 InEnsureTriggerNotify = 0, EAnimNotifyProType InNotifyType = EAnimNotifyProType::Notify, float InTime = 0.f)
		: EnsureTriggerNotify(InEnsureTriggerNotify)
		, bEnsureEndStateIfTriggered(true)
		, Time(InTime)
		, NotifyId(InNotifyId)
		, bHasBroadcast(false)
		, bIsEndState(false)
		, bNotifySkipped(false)
		, NotifyStatePair(nullptr)
		, NotifyType(InNotifyType)
	{}

	UPROPERTY()
	int32 EnsureTriggerNotify;

	UPROPERTY()
	bool bEnsureEndStateIfTriggered;

	UPROPERTY()
	float Time;

	UPROPERTY()
	uint32 NotifyId;

	UPROPERTY()
	bool bHasBroadcast;

	UPROPERTY()
	bool bIsEndState;

	UPROPERTY()
	bool bNotifySkipped;
	
	TObjectPtr<FAnimNotifyProEvent> NotifyStatePair;

	EAnimNotifyProType NotifyType;

	FTimerHandle Timer;
	FTimerDelegate TimerDelegate;

	UPROPERTY()
	TWeakObjectPtr<UAnimNotifyPro> Notify;

	UPROPERTY()
	TWeakObjectPtr<UAnimNotifyStatePro> NotifyState;

	void OnTimer();
	
	void ClearTimers()
	{
		if (Timer.IsValid())
		{
			Timer.Invalidate();
		}
		if (TimerDelegate.IsBound())
		{
			TimerDelegate.Unbind();
		}
	}

	bool IsValid() const { return NotifyId > 0 && (Notify.IsValid() || NotifyState.IsValid()); }

	bool operator==(const FAnimNotifyProEvent& Other) const
	{
		return NotifyId > 0 && NotifyId == Other.NotifyId;
	}

	bool operator!=(const FAnimNotifyProEvent& Other) const
	{
		return !(*this == Other);
	}
};