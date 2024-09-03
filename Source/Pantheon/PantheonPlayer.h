// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "InputActionValue.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/Character.h"
#include "PantheonPlayer.generated.h"

UCLASS()
class PANTHEONGAME_API APantheonPlayer : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	APantheonPlayer();

	float MovementSpeed = 500;
	float RightValue = 0;
	float ForwardValue = 0;
	float RotationSpeed = 30;


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	 USpringArmComponent* SpringArmComponent;


	 UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
    UCameraComponent* CameraComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mesh", meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* PlayerMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mesh", meta = (AllowPrivateAccess = "true"))
	UCapsuleComponent* Capsule = nullptr;

	UPROPERTY(EditAnywhere, Category = "EnhancedInput")
		class UInputMappingContext* InputMapping;

		UPROPERTY(EditAnywhere, Category = "EnhancedInput")
		class UInputAction *TestAction;

		UPROPERTY(EditAnywhere, Category = "EnhancedInput")
		class UInputAction *ForwardAction;

		UPROPERTY(EditAnywhere, Category = "EnhancedInput")
		class UInputAction *BackwardAction;

		UPROPERTY(EditAnywhere, Category = "EnhancedInput")
		class UInputAction *LeftAction;

		UPROPERTY(EditAnywhere, Category = "EnhancedInput")
		class UInputAction *RightAction;

		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animations")
UAnimationAsset* IdleAnimation;

UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animations")
UAnimationAsset* MovingAnimation;

UAnimationAsset* CurrentAnimation;

		FVector CameraForward;
		FVector CameraRight;
		FRotator SavedRotation;
		FRotator TrueRotation;


		void TestInput();

		virtual void MoveRight();
		virtual void MoveLeft();
	
	  	virtual void MoveBackward();
	 	virtual void MoveForward();
	    virtual void AddInput(FVector WorldDirection, float ScaleValue);

		virtual void StopForwardMovement();
		virtual void StopRightMovement();

		virtual void PlayAnimationOnce(UAnimationAsset* AnimationToPlay, bool loopAnimation);

};
