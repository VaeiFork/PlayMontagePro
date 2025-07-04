// Copyright (c) Jared Taylor

#pragma once

#include "CoreMinimal.h"
#include "PlayMontageTypes.h"
#include "UObject/Interface.h"
#include "PlayMontageProInterface.generated.h"

UINTERFACE()
class UPlayMontageProInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * @TODO desc
 */
class PLAYMONTAGEPRO_API IPlayMontageProInterface
{
	GENERATED_BODY()

public:
	virtual void BroadcastNotifyEvent(FAnimNotifyProEvent& Event) = 0;
	
	virtual void NotifyCallback(const FAnimNotifyProEvent& Event) = 0;
	virtual void NotifyBeginCallback(const FAnimNotifyProEvent& Event) = 0;
	virtual void NotifyEndCallback(const FAnimNotifyProEvent& Event) = 0;

	virtual UAnimMontage* GetMontage() const = 0;
	virtual USkeletalMeshComponent* GetMesh() const = 0;

	virtual FTimerDelegate CreateTimerDelegate(FAnimNotifyProEvent& Event) = 0;
	void OnNotifyTimer(FAnimNotifyProEvent* Event) { BroadcastNotifyEvent(*Event); }
};
