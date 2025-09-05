# 숫자 야구 게임 (Number Baseball Game)

## 🎮 게임 개요

숫자 야구는 상대방이 정한 3자리 숫자를 맞추는 게임입니다. 각 플레이어는 최대 7번의 기회를 가지며, Strike와 Ball의 힌트를 통해 정답을 추리해야 합니다.

### 게임 규칙

- **정답 숫자**: 1-9 사이의 서로 다른 3개 숫자 (0 제외)
- **최대 시도 횟수**: 7회
- **Strike**: 숫자와 위치가 모두 일치
- **Ball**: 숫자는 일치하지만 위치가 틀림
- **Out**: Strike와 Ball이 모두 없는 경우

## ✨ 필수 기능

- ✅ **GameMode 서버 로직**: 멀티플레이어 게임 상태 관리
- ✅ **난수 생성 로직**: 중복 없는 3자리 숫자 자동 생성
- ✅ **판정 로직**: Strike/Ball/Out 자동 판별
- ✅ **시도 횟수 및 상태 관리**: 플레이어별 시도 횟수 추적
- ✅ **승리/무승부/게임 리셋**: 게임 종료 조건 및 자동 재시작

## 🐛 해결된 주요 이슈

### 1. 플레이어 접속 메시지 중복 문제
**문제상황:**
- 플레이어 접속 시 "Player1: Player2 has joined the game." 같은 형태로 출력
- 접속 메시지가 중첩되어 표시되는 현상

**원인 분석:**
- GameState에서 Multicast 요청 시 `SetChatMessageString()` 사용
- 이로 인해 플레이어가 직접 입력한 메시지로 처리됨

**해결 방법:**
```cpp
//기존 코드 (문제 상황)
SetChatMessageString(NotificationString);

//수정된 코드
ClientRPCPrintChatMessageString(NotificationString);
```
`ClientRPCPrintChatMessageString()`을 직접 호출하여 접속 알림만 출력되도록 수정

### 2. 시도 횟수 실시간 반영 문제
**문제상황:**
- 플레이어의 시도 횟수가 메시지에 즉시 반영되지 않는 현상
- Player(n/n) 형태의 정보가 이전 상태로 표시

**원인 분석:**
- 플레이어 정보가 시도 이전에 미리 반영되어 있었음
- 입력 메시지와 조합하는 시점의 문제

**해결 방법:**
```cpp
//GameModeBase에서 직접 최신 플레이어 정보 조합
FString PlayerInfo = NBPC->GetPlayerState()->GetPlayerInfoString();
FString CombinedMessageString = PlayerInfo + TEXT(": ") + InChatMessageString + TEXT(" -> ") + JudgeResultString;
```
GameModeBase에서 실시간으로 플레이어 정보를 받아와 텍스트 조합 후 전송
