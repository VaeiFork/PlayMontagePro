// Copyright (c) Jared Taylor

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "PMPGameplayAbility.generated.h"

// Most of this is from GASShooter and therefore also Copyright 2024 Dan Kestranek.
// https://github.com/tranek/GASShooter

USTRUCT()
struct PLAYMONTAGEPRO_API FAbilityMeshMontage
{
	GENERATED_BODY()

	UPROPERTY()
	USkeletalMeshComponent* Mesh;

	UPROPERTY()
	UAnimMontage* Montage;

	FAbilityMeshMontage(USkeletalMeshComponent* InMesh = nullptr, UAnimMontage* InMontage = nullptr) 
		: Mesh(InMesh)
		, Montage(InMontage)
	{
	}
};

/**
 * 
 */
UCLASS()
class PLAYMONTAGEPRO_API UPMPGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()

public:
	UPMPGameplayAbility(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	// ----------------------------------------------------------------------------------------------------------------
	//	Animation Support for multiple USkeletalMeshComponents on the AvatarActor
	// ----------------------------------------------------------------------------------------------------------------

	/** Active montages being played by this ability */
	UPROPERTY()
	TArray<FAbilityMeshMontage> CurrentAbilityMeshMontages;

	bool FindAbilityMeshMontage(const USkeletalMeshComponent* InMesh, FAbilityMeshMontage& InAbilityMontage);
	
	/** Returns the currently playing montage for this ability, if any */
	UFUNCTION(BlueprintCallable, Category = Animation)
	UAnimMontage* GetCurrentMontageForMesh(USkeletalMeshComponent* InMesh);

	/** Call to set/get the current montage from a montage task. Set to allow hooking up montage events to ability events */
	virtual void SetCurrentMontageForMesh(USkeletalMeshComponent* InMesh, class UAnimMontage* InCurrentMontage);

};
