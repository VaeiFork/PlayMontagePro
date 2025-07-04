// Copyright (c) Jared Taylor

#pragma once

#include "CoreMinimal.h"
#include "PlayMontageTypes.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AnimNotifyPro.generated.h"

/**
 * @TODO desc
 */
UCLASS(Abstract, Blueprintable, const)
class PLAYMONTAGEPRO_API UAnimNotifyPro : public UAnimNotify
{
	GENERATED_BODY()

public:
	/** Ensure that notifies are triggered if the montage aborts before they're reached when aborted due to these conditions */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=AnimNotify)
	TArray<EAnimNotifyProEventType> EnsureTriggerNotify = {};

	/** If disabled this notify will be skipped on dedicated servers */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=AnimNotify)
	bool bTriggerOnDedicatedServer = true;

	/** SimulatedProxies do not typically execute PlayMontage nodes and need to route through the legacy notify system */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=AnimNotify)
	EAnimNotifyLegacyType SimulatedProxyBehavior = EAnimNotifyLegacyType::Legacy;

#if WITH_EDITORONLY_DATA

protected:
	bool bHasBlueprintReceivedNotify;
	
#endif
	
public:
	UAnimNotifyPro(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

#if WITH_EDITOR
	virtual EDataValidationResult IsDataValid(class FDataValidationContext& Context) const override;
#endif
	
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override final;
	virtual void BranchingPointNotify(FBranchingPointNotifyPayload& BranchingPointPayload) override final {}

public:
	virtual bool ShouldTriggerNotify(USkeletalMeshComponent* MeshComp) const;
	
	virtual void NotifyCallback(USkeletalMeshComponent* MeshComp, UAnimMontage* Montage);
	virtual void OnNotify(USkeletalMeshComponent* MeshComp, UAnimMontage* Montage);
	
	UFUNCTION(BlueprintImplementableEvent, meta=(DisplayName="On Notify"))
	bool K2_OnNotify(USkeletalMeshComponent* MeshComp, UAnimMontage* Montage) const;

#if WITH_EDITOR
	virtual bool CanBePlaced(UAnimSequenceBase* Animation) const override
	{
		return Animation && Animation->IsA(UAnimMontage::StaticClass());
	}
#endif
};
