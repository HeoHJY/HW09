#include "Game/NBGameModeBase.h"
#include "NBGameStateBase.h"
#include "Player/NBPlayerController.h"
#include "EngineUtils.h"
#include "Player/NBPlayerState.h"

void ANBGameModeBase::BeginPlay()
{
	Super::BeginPlay();

	AnswerNumberString = GenerateAnswerNumber();
	UE_LOG(LogTemp, Error, TEXT("AnswerNumberString : %s"), *AnswerNumberString);
}

void ANBGameModeBase::OnPostLogin(AController* NewPlayer)
{
	Super::OnPostLogin(NewPlayer);

	//컨트롤러로 형변환
	ANBPlayerController* NBPC = Cast<ANBPlayerController>(NewPlayer);
	if (IsValid(NBPC) == true)
	{
		//접속 성공 텍스트 출력
		NBPC->NotificationText = FText::FromString(TEXT("Connected to the game server."));
		
		//플레이어 리스트에 추가
		AllPlayerControllers.Add(NBPC);

		//플레이어 스테이트에 이름 추가
		ANBPlayerState* NBPS = NBPC->GetPlayerState<ANBPlayerState>();
		if (IsValid(NBPS) == true)
		{
			NBPS->PlayerNameString = TEXT("Player") + FString::FromInt(AllPlayerControllers.Num());
		}

		//로그인 메시지 출력
		ANBGameStateBase* NBGS = GetGameState<ANBGameStateBase>();
		if (IsValid(NBGS) == true)
		{
			NBGS->MulticastRPCBroadcastLoginMessage(NBPS->PlayerNameString);
		}
	}
}

FString ANBGameModeBase::GenerateAnswerNumber()
{
	TArray<int32> Numbers;
	for (int32 i = 0; i < 10; ++i)
	{
		Numbers.Add(i);
	}

	//현재 시간의 틱 값으로 난수 시드 설정
	FMath::RandInit(FDateTime::Now().GetTicks());
	//0이 등장하지 않게 설정
	Numbers = Numbers.FilterByPredicate([](int32 Num) { return Num > 0;});

	FString Result;
	for (int32 i = 0; i < 3; ++i)
	{
		int32 Index = FMath::RandRange(0, Numbers.Num() - 1); //범위 내의 랜덤 인덱스 생성
		Result += FString::FromInt(Numbers[Index]); //해당 인덱스의 값 추가
		Numbers.RemoveAt(Index); //중복 숫자 방지를 위해 사용된 숫자 제거
	}

	return Result;
}

bool ANBGameModeBase::IsGuessNumberString(const FString& InNumberString)
{
	bool bCanPlay = false;

	do {
		//길이가 3이 아니라면 false
		if (InNumberString.Len() != 3)
		{
			break;
		}

		
		bool bIsUnique = true;
		TSet<TCHAR> UniqueDigits; //중복 원소 방지를 위한 Set
		for (TCHAR C : InNumberString)
		{
			//숫자가 아니거나 0이면 false
			if (FChar::IsDigit(C) == false || C == '0')
			{
				bIsUnique = false;
				break;
			}

			UniqueDigits.Add(C);
		}

		//IsUnique가 false거나 Set의 크기가 3이 아니라면 false
		if (bIsUnique == false || UniqueDigits.Num() != 3)
		{
			break;
		}

		//모든 조건을 충족하면 true
		bCanPlay = true;
	} while (false);

	return bCanPlay; 
}

FString ANBGameModeBase::JudgeResult(const FString& InAnswerNumberString, const FString& InGuessNumberString)
{
	int32 StrikeCount = 0, BallCount = 0;

	for (int32 i = 0; i < 3; ++i)
	{
		//숫자가 정확히 일치하면 Strike +1
		if (InAnswerNumberString[i] == InGuessNumberString[i])
		{
			StrikeCount++;
		}
		else
		{
			//숫자가 정답 숫자에 포함되어 있다면 Ball +1
			FString PlayerGuessChar = FString::Printf(TEXT("%c"), InGuessNumberString[i]);
			if (InAnswerNumberString.Contains(PlayerGuessChar) == true)
			{
				BallCount++;
			}
		}
	}
	
	//맞은 것이 하나도 없을 경우 Out처리
	if (StrikeCount == 0 && BallCount == 0)
	{
		return TEXT("OUT!");
	}

	//아니라면 Strike와 Ball의 카운트 출력
	return FString::Printf(TEXT("%dS/%dB"), StrikeCount, BallCount);
}

void ANBGameModeBase::PrintChatMessageString(ANBPlayerController* InChattingPlayerController, const FString& InChatMessageString)
{
	//입력된 메시지의 마지막 3글자를 저장
	int Index = InChatMessageString.Len() - 3;
	FString GuessNumberString = InChatMessageString.RightChop(Index);
	
	//정답을 맞추기 위해 입력한 거라면
	if (IsGuessNumberString(GuessNumberString) == true)
	{
		//시도 횟수 증가
		IncreaseGuessCount(InChattingPlayerController);
		//정답 판단
		FString JudgeResultString = JudgeResult(AnswerNumberString, GuessNumberString);
		//플레이어 탐색
		for (TActorIterator<ANBPlayerController> It(GetWorld()); It; ++It)
		{
			ANBPlayerController* NBPC = *It;
			FString PlayerInfo = NBPC->GetPlayerState<ANBPlayerState>()->GetPlayerInfoString();
			
			if (IsValid(NBPC) == true)
			{
				//결과 전달
				FString CombinedMessageString = PlayerInfo + TEXT(": ") + InChatMessageString + TEXT(" -> ") + JudgeResultString;
				NBPC->ClientRPCPrintChatMessageString(CombinedMessageString);

				//Strike 숫자만 나오도록 분리
				int32 StrikeCount = FCString::Atoi(*JudgeResultString.Left(1));
				//결과 판정
				JudgeGame(InChattingPlayerController, StrikeCount);
			}
		}
	}
	//아니라면 단순 메시지 출력
	else
	{
		for (TActorIterator<ANBPlayerController> It(GetWorld()); It; ++It)
		{
			ANBPlayerController* NBPC = *It;
			FString PlayerInfo = NBPC->GetPlayerState<ANBPlayerState>()->GetPlayerInfoString();
			FString CombinedMessageString = PlayerInfo + TEXT(": ") + InChatMessageString;
			
			if (IsValid(NBPC) == true)
			{
				NBPC->ClientRPCPrintChatMessageString(CombinedMessageString);
			}
		}
	}
}

void ANBGameModeBase::IncreaseGuessCount(ANBPlayerController* InPlayerController)
{
	ANBPlayerState* NBPS = InPlayerController->GetPlayerState<ANBPlayerState>();
	if (IsValid(NBPS) == true)
	{
		NBPS->CurrentGuessCount++;
	}
}

void ANBGameModeBase::ResetGame()
{
	//정답 숫자 새로 생성
	AnswerNumberString = GenerateAnswerNumber();

	//모든 플레이어를 탐색
	for (const auto& NBPC : AllPlayerControllers)
	{
		ANBPlayerState* NBPS = NBPC->GetPlayerState<ANBPlayerState>();
		if (IsValid(NBPS) == true)
		{
			NBPS->CurrentGuessCount = 0; //시도 횟수 0으로 초기화 
		}
	}
}

void ANBGameModeBase::JudgeGame(ANBPlayerController* InChattingPlayerController, int32 InStrikeCount)
{
	//맞췄을 경우
	if (InStrikeCount == 3)
	{
		//플레이어 탐색
		ANBPlayerState* NBPS = InChattingPlayerController->GetPlayerState<ANBPlayerState>();
		for (const auto& NBPC : AllPlayerControllers)
		{
			if (IsValid(NBPS) == true)
			{
				//승리 처리
				FString CombinedMessageString = NBPS->PlayerNameString + TEXT(" has won the game.");
				NBPC->NotificationText = FText::FromString(CombinedMessageString);
				//게임 재시작
				ResetGame();
			}
		}
	}
	else
	{
		bool bIsDraw = true;
		for (const auto& NBPC : AllPlayerControllers)
		{
			ANBPlayerState* NBPS = NBPC->GetPlayerState<ANBPlayerState>();
			if (IsValid(NBPS) == true)
			{
				//모든 시도 횟수를 사용하지 않았을 경우
				if (NBPS->CurrentGuessCount < NBPS->MaxGuessCount)
				{
					//무승부가 아님
					bIsDraw = false;
					break;
				}
			}
		}

		//무승부일 경우
		if (bIsDraw == true)
		{
			for (const auto& NBPC : AllPlayerControllers)
			{
				//무승부 처리
				NBPC->NotificationText = FText::FromString(TEXT("Draw The Game."));
				//게임 재시작
				ResetGame();
			}
		}
	}
}