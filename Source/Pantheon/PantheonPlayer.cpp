// Fill out your copyright notice in the Description page of Project Settings.


#include "PantheonPlayer.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/InputComponent.h"
#include "UObject/ConstructorHelpers.h"

// Sets default values
APantheonPlayer::APantheonPlayer()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	 Capsule = GetCapsuleComponent();
	  Capsule->InitCapsuleSize(42.0f, 96.0f);

    // Enable collision
   Capsule->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    Capsule->SetCollisionObjectType(ECC_Pawn);
    Capsule->SetSimulatePhysics(true);
    Capsule->SetEnableGravity(false);


	 SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	 
    SpringArmComponent->SetupAttachment(RootComponent);
    SpringArmComponent->TargetArmLength = 800.0f; // Camera distance
    SpringArmComponent->SetWorldRotation(FRotator(-28.f, -34.f, 0.f)); // Isometric angle
	SpringArmComponent->bUsePawnControlRotation = false; // Ensure the pawn's rotation doesn't affect the spring arm

    // Create a camera and attach to spring arm
    CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
    CameraComponent->SetupAttachment(SpringArmComponent, USpringArmComponent::SocketName);
    CameraComponent->bUsePawnControlRotation = false; // Don't rotate the camera with the pawn

    // Ensure the camera uses orthographic projection for isometric view
    CameraComponent->SetProjectionMode(ECameraProjectionMode::Orthographic);
    CameraComponent->OrthoWidth = 2048.0f; // Adjust as needed for your scene

	CameraComponent->SetRelativeRotation(FRotator(-28.f, -34.f, 0.f));
    


    // static ConstructorHelpers::FObjectFinder<USkeletalMesh> MeshAsset(TEXT("/Game/ControlRig/Characters/Mannequins/Meshes/SKM_Manny"));
    PlayerMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh"));
    // PlayerMesh->SkeletalMesh = MeshAsset.Object;
    PlayerMesh->SetupAttachment(SpringArmComponent, USpringArmComponent::SocketName);
    PlayerMesh->SetWorldRotation(FRotator(28,0,0));

	float CameraYaw = -34.0f;
	float CameraPitch = -28.0f;

	// Convert angles from degrees to radians
	float YawRadians = FMath::DegreesToRadians(CameraYaw);
	float PitchRadians = FMath::DegreesToRadians(CameraPitch);


	 CameraForward =FVector(
    FMath::Cos(PitchRadians) * FMath::Cos(YawRadians),
    FMath::Cos(PitchRadians) * FMath::Sin(YawRadians),
    FMath::Sin(PitchRadians)
);

	 CameraRight = FVector(
    -FMath::Sin(YawRadians),
    FMath::Cos(YawRadians),
    0.0f
);
	    CameraForward.Z = 0; // Ignore vertical component
		 CameraRight.Z = 0;
	

	SavedRotation = FRotator::ZeroRotator;
	TrueRotation = FRotator::ZeroRotator;

	CurrentAnimation = nullptr;
}

// Called when the game starts or when spawned
void APantheonPlayer::BeginPlay()
{
	Super::BeginPlay();


	if(APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if(UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
        {
			Subsystem->AddMappingContext(InputMapping,0);
		}
	}
	
}

// Called every frame
void APantheonPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);


		//   UE_LOG(LogTemp, Warning, TEXT("The saved rotation is %s"), *SavedRotation.ToString());

		FVector MovementDirection = FVector(ForwardValue, RightValue, 0.0f);
    if(!MovementDirection.IsNearlyZero())
    {
          FRotator NewRotation = FMath::RInterpTo(PlayerMesh->GetComponentRotation(), TrueRotation, DeltaTime , RotationSpeed);
                         // Apply the new rotation

        	PlayerMesh->SetWorldRotation(NewRotation);

        MovementDirection = MovementDirection.GetClampedToMaxSize(1.0f);
		  if (CurrentAnimation != MovingAnimation)
        {
            PlayAnimationOnce(MovingAnimation,true);
            CurrentAnimation = MovingAnimation;
        }
        
    }
    else
    {
		   if (CurrentAnimation != IdleAnimation)
        {
            PlayAnimationOnce(IdleAnimation,true);
            CurrentAnimation = IdleAnimation;
        }
       Capsule->SetPhysicsLinearVelocity(FVector::ZeroVector);
    }

	
}

// Called to bind functionality to input
void APantheonPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if(UEnhancedInputComponent* Input = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		Input->BindAction(TestAction, ETriggerEvent::Triggered, this, &APantheonPlayer::TestInput);
		Input->BindAction(ForwardAction, ETriggerEvent::Triggered, this, &APantheonPlayer::MoveForward);
		Input->BindAction(BackwardAction, ETriggerEvent::Triggered, this, &APantheonPlayer::MoveBackward);
		Input->BindAction(RightAction, ETriggerEvent::Triggered, this, &APantheonPlayer::MoveRight);
		Input->BindAction(LeftAction, ETriggerEvent::Triggered, this, &APantheonPlayer::MoveLeft);


		
		
		Input->BindAction(ForwardAction, ETriggerEvent::Completed, this, &APantheonPlayer::StopForwardMovement);
		Input->BindAction(BackwardAction, ETriggerEvent::Completed, this, &APantheonPlayer::StopForwardMovement);
		Input->BindAction(RightAction, ETriggerEvent::Completed, this, &APantheonPlayer::StopRightMovement);
		Input->BindAction(LeftAction, ETriggerEvent::Completed, this, &APantheonPlayer::StopRightMovement);

	}



}

void APantheonPlayer::PlayAnimationOnce(UAnimationAsset* AnimationToPlay, bool loopAnimation)
{
    if (AnimationToPlay && PlayerMesh)
    {
        // Play the animation once
        PlayerMesh->PlayAnimation(AnimationToPlay, loopAnimation);

        // Optionally, you can add a callback or a delay to do something after the animation finishes
        // For example, you could stop the animation after its length:
        // GetWorld()->GetTimerManager().SetTimer(AnimationTimerHandle, this, &APantheonPlayer::OnAnimationFinished, AnimationToPlay->GetMaxCurrentTime(), false);
    }
}

void APantheonPlayer::TestInput()
{
	GEngine->AddOnScreenDebugMessage(-1,1,FColor::Red,"Pressed Input Action");
}

void APantheonPlayer::StopRightMovement()
{
    RightValue = 0;
    AddInput(GetActorForwardVector(), RightValue);
    
}

void APantheonPlayer::StopForwardMovement()
{
    ForwardValue = 0;
    AddInput(GetActorForwardVector(), RightValue);
    
}

void APantheonPlayer::MoveLeft(){
 
    RightValue = 1;
       FVector Direction = (CameraForward * ForwardValue) + (CameraRight * RightValue);
    Direction = Direction.GetSafeNormal();
    AddInput(Direction, 1.0f);
}

void APantheonPlayer::MoveRight(){
   RightValue = -1;

      FVector Direction = (CameraForward * ForwardValue) + (CameraRight * RightValue);
    Direction = Direction.GetSafeNormal();
    AddInput(Direction, 1.0f);
}

void APantheonPlayer::MoveBackward(){
    
    ForwardValue = 1;
	 FVector Direction = (CameraForward * ForwardValue) + (CameraRight * RightValue);
    Direction = Direction.GetSafeNormal();
       AddInput(Direction, ForwardValue);
}

void APantheonPlayer::MoveForward(){
    
      ForwardValue = -1;
	   FVector Direction = (CameraForward * ForwardValue) + (CameraRight * RightValue);
    Direction = Direction.GetSafeNormal();
      AddInput(Direction, ForwardValue);
}

void APantheonPlayer::AddInput(FVector WorldDirection, float ScaleValue)
{
    if (!WorldDirection.IsNearlyZero())
    {
        FVector Velocity = WorldDirection * MovementSpeed;
      
	   if (!WorldDirection.IsNearlyZero()) 
    {
		FVector Direction = WorldDirection.GetSafeNormal();
		UE_LOG(LogTemp, Warning, TEXT("Before Setting: SavedRotation: %s"), *SavedRotation.ToString());

        SavedRotation = Direction.Rotation();
		SavedRotation.Yaw += 270.0f;
	
		SavedRotation.Yaw = FMath::Fmod(SavedRotation.Yaw, 360.0f);
		if (SavedRotation.Yaw < 0)
		{
   			 SavedRotation.Yaw += 360.0f;
		}

			if(SavedRotation != FRotator(0,90,0))
				{
                       TrueRotation = SavedRotation;
                }

        
		
    }
       
        Capsule->SetPhysicsLinearVelocity(Velocity);
          FVector Speed =Capsule->GetPhysicsLinearVelocity();
       
    }
} 

