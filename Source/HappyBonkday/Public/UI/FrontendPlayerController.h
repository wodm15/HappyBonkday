// JY Sun All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "FrontendPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class HAPPYBONKDAY_API AFrontendPlayerController : public APlayerController
{
	GENERATED_BODY()
	
	// APlayerController 인터페이스 시작
	virtual void OnPossess(APawn* aPawn) override;

	// APlayerController 인터페이스 종료료
};
