# 과제 체크리스트 순서별 검사

검사일: 2026-10-27. 현재 소스와 저장된 UE 5.6 애셋을 직접 읽고 `CX.Gameplay.AttackInputMapping`, `CX.Gameplay.ServerRules`를 재실행했다. 두 테스트 모두 Success, 종료 코드 0이다. 검사 중 게임 구현 코드는 변경하지 않았다.

**판정 기준**: ✅는 해당 요구사항을 코드·실제 저장 애셋·해당 범위의 자동 테스트로 확인했다는 뜻이다. △는 구현은 있으나 화면/사운드/실제 다중 클라이언트 동작 또는 좌표 적합성까지 독립적으로 확인하지 못했다는 뜻이다. △를 미구현이나 실패로 해석하지 않는다. 이전 사용자의 “나머지는 다 괜찮음” 확인은 참고했지만, 독립 검증을 대신하지 않았다.

원래 사용자 체크박스 47개 기준 **✅ 36개 / △ 11개**. 추가로 OwnerOnly 비노출의 실제 네트워크 검증 범위도 아래에 명시했다. 모든 항목을 실행 완료했다고 단정할 근거는 아직 없다.

## 1. 채팅 시스템 (원래 순서)

| 번호 | 요구사항 | 판정 | 확인 근거 / 남은 확인 |
|---|---|---|---|
| 1 | 채팅창 위젯 클래스 | ✅ | `UChatUI`, UserWidget 상속 |
| 2 | 위젯 블루프린트 애셋 | ✅ | `/Game/CXMultiplayer/UI/WBP_Chat` 실제 로드 성공 |
| 3 | 위젯 객체 생성 및 연결 | ✅ | 컨트롤러 ChatUIClass가 WBP_Chat, 로컬 BeginPlay에서 CreateWidget/AddToViewport |
| 4 | 엔터 이벤트 로직 | ✅ | OnTextCommitted 바인딩, OnEnter 처리, 입력 초기화와 운전 모드 복귀 |
| 5 | 작성한 메시지를 화면에 표시 | △ | OnNewChatMessage가 로그 갱신. 최종 화면 렌더링은 이번 검사에서 미실행 |
| 6 | 채팅 동기화 | △ | Server RPC→모든 컨트롤러 Client RPC 경로 확인. 실제 두 클라이언트의 메시지 왕복을 자동 테스트하지 않음 |

## 2. PlayerState (원래 순서)

| 번호 | 요구사항 | 판정 | 확인 근거 / 남은 확인 |
|---|---|---|---|
| 1 | Coins 속성 및 레플리케이션 | ✅ | Replicated 속성, GetLifetimeReplicatedProps 등록 |
| 2 | 원화 증가·차감 함수 | ✅ | AddCoins/RemoveCoins, 증감·잔액 부족·음수 거부 테스트 |
| 3 | 원화는 서버에서만 변경 | ✅ | HasAuthority 검사, SimulatedProxy의 증감 요청 거부 테스트 |
| 4 | Coins의 COND_OwnerOnly 작성 | ✅ | DOREPLIFETIME_CONDITION(ACXPlayerState, Coins, COND_OwnerOnly) |
| 5 | BonusMaxSpeed 속성 및 레플리케이션 | ✅ | Replicated 속성 및 복제 등록 |
| 6 | 추가 최대 속도 증가 함수 | ✅ | AddBonusMaxSpeed 및 +10 테스트 |
| 7 | 추가 최대 속도는 서버에서만 변경 | ✅ | HasAuthority 검사, SimulatedProxy의 증가 거부 테스트 |
| 8 | BonusMaxSpeed의 COND_OwnerOnly 작성 | ✅ | 해당 DOREPLIFETIME_CONDITION 등록 |
| 9 | 자신의 원화·속도 보너스 UI | △ | 로컬 컨트롤러가 자신의 PlayerState만 읽음. 실제 두 HUD 비교는 별도 확인 필요 |

OwnerOnly는 요구된 코드 작성까지 확인했다. 소유 클라이언트에는 변경값이 도착하고 다른 클라이언트의 동일 PlayerState에는 값이 노출되지 않는지 확인하는 실제 네트워크 검사는 기존 자동 테스트에 포함되어 있지 않다.

## 3. Vehicle 및 아이템 (원래 순서)

| 번호 | 요구사항 | 판정 | 확인 근거 / 남은 확인 |
|---|---|---|---|
| 1 | 좌클릭 InputAction 애셋·차량 연결 | ✅ | IA_Attack 실제 저장, IMC_Default의 LeftMouseButton 매핑 1개, 스포츠카/오프로드 연결. AttackInputMapping 테스트 통과 |
| 2 | 좌클릭 시 차량 Server RPC 호출 | ✅ | Enhanced Input Started→Attack→ServerAttack 코드 연결. 이번 테스트는 물리 마우스 클릭을 주입하지 않음 |
| 3 | 앞 범퍼 중앙에서 서버 라인트레이스 | △ | AttackOrigin을 시작점으로 전방 트레이스 구현. 기본 로컬 (220,0,40)이 차종별 실제 범퍼 중앙에 맞는지는 시각 확인 필요 |
| 4 | 상대 차량 감지 및 ApplyDamage | ✅ | 실제 라인트레이스 ServerAttack 호출로 코인 이전까지 자동 테스트 통과 |
| 5 | 가해 차량 +5원 / 피해 차량 -5원 | ✅ | 자동 테스트에서 10→5, 0→5 확인 |
| 6 | 피해자 5원 미만이면 이전 없음 | ✅ | 피해자 4원일 때 양쪽 잔액 불변 테스트 |
| 7 | DrawDebugLine 및 피격 파티클 | △ | multicast와 파티클 애셋 연결 확인. NullRHI 테스트로 실제 선/입자 렌더링은 확인 불가 |
| 8 | 코인 액터, 오버랩 +10원 | ✅ | ACXCoin 및 BP_Coin, 실제 오버랩 +10 테스트 |
| 9 | 코인 NetUpdateFrequency=1 | ✅ | 저장된 BP_Coin 기본값 1.0 |
| 10 | 오버랩된 코인 파괴 | ✅ | 오버랩 후 IsActorBeingDestroyed 테스트 |
| 11 | 코인 파괴 시 파티클·사운드 | △ | 획득 처리에서 효과 multicast 후 Destroy. 파티클/사운드 실애셋 연결 확인. 실제 재생 확인 필요 |
| 12 | 스피드 버프 액터, 오버랩 +10 | ✅ | ACXSpeedBuff 및 BP_SpeedBuff, 실제 오버랩 +10 테스트 |
| 13 | 버프 NetUpdateFrequency=1 | ✅ | 저장된 BP_SpeedBuff 기본값 1.0 |
| 14 | 오버랩된 버프 파괴 | ✅ | 오버랩 후 IsActorBeingDestroyed 테스트 |
| 15 | 버프 파괴 시 파티클·사운드 | △ | 획득 multicast 및 실제 효과/사운드 애셋 연결. 실제 재생 확인 필요 |
| 16 | 스포너 클래스 및 스폰 로직 | ✅ | ACXItemSpawner와 BP_ItemSpawner, 서버 생성 테스트 |
| 17 | 생성 아이템 참조·OnDestroyed 구독 | ✅ | SpawnedItem 저장·AddDynamic, 파괴 후 참조 초기화 테스트 |
| 18 | 2초 후 같은 종류 리스폰 | ✅ | ItemClass 재사용, RespawnDelay=2.0. 1초 시 미생성 / 누적 2.1초 후 생성 테스트. 자동 테스트의 실제 재생성 대상은 기본 코인 |
| 19 | 레벨 스포너 배치·코인/버프 종류 지정 | ✅ | VehicleAdvExampleMap 실애셋에서 코인 6개/버프 3개, 전부 2.0초 설정 확인. 오프로드 맵에는 별도 배치 없음 |

아이템은 bReplicates=true 및 bAlwaysRelevant=true, 스폰·획득·파괴는 서버 권한으로 구현되어 있다. 현재 두 클라이언트 간 아이템 동시 소멸/재등장의 실제 네트워크 전달은 이번 단일 월드 규칙 테스트의 범위 밖이다.

효과는 **획득에 따른 파괴 경로**에서 재생된다. 다른 코드나 콘솔이 임의로 Destroy하는 모든 경우에 효과를 재생하도록 구현된 것은 아니다. 과제의 “파괴 시”가 임의 파괴까지 포함하는 엄격한 의미라면 이 부분은 추가 구현 대상이다.

## 4. GameMode / GameState (원래 순서)

| 번호 | 요구사항 | 판정 | 확인 근거 / 남은 확인 |
|---|---|---|---|
| 1 | GameState 클래스 생성 | ✅ | ACXGameState, 실제 GameMode의 GameStateClass 연결 |
| 2 | Waiting/Playing/Finished Enum | ✅ | ECXMatchPhase 정의 |
| 3 | 현재 상태·남은 시간 속성 | △ | Replicated와 복제 등록 확인. 모든 클라이언트에 동일 값이 도착하는지 독립 네트워크 검증 필요 |
| 4 | 우승자 목록 속성 | ✅ | Replicated Winners 배열 및 복제 등록 |
| 5 | 시작 시 Waiting | ✅ | InitGameState가 Waiting, 남은 시간 초기화 |
| 6 | 첫 접속 시 대기 카운트다운 | ✅ | 로그인 전 시간 불변, PostLogin 후 타이머 시작 테스트 |
| 7 | 서버 1초 대기 감소 및 UI | △ | 1초 서버 타이머와 UI 값 읽기 확인. 실제 두 화면의 표시까지 이번 검사에서 미실행 |
| 8 | 대기 종료 시 Playing | ✅ | 자동 테스트 통과 |
| 9 | 서버 1초 플레이 감소 및 UI | △ | 같은 타이머로 감소·표시 구현. 실제 표시와 네트워크 전달은 추가 확인 |
| 10 | 플레이 종료 시 Finished | ✅ | 자동 테스트 통과, 종료 시 타이머 해제 |
| 11 | 최고 원화 플레이어 탐색 | ✅ | 서버 PlayerArray 전체 탐색, 더 큰 값이면 기존 우승자 초기화. 현재 자동 테스트의 최종 판정 사례는 동점 |
| 12 | 동점자 전원 공동 우승 | ✅ | 두 플레이어 동점 시 Winners 2명 테스트 |
| 13 | 로컬 승리·패배 결과 UI | △ | 로컬 PS가 Winners에 포함되면 VICTORY, 아니면 DEFEAT. 실제 두 화면의 최종 표시 확인 필요 |

## 증거와 한계

- 소스: `Source/CX/UI/ChatUI.*`, `CXPlayerController.*`, `CXPlayerState.*`, `CXPawn.*`, `CXPickup.*`, `CXItemSpawner.*`, `CXGameMode.*`, `CXGameState.*`.
- 현재 저장 애셋 감사: `Saved/Audit/checklist_assets.json`.
- 검사 로그: `Saved/Logs/CXChecklistAudit.log` (0 errors / 0 warnings).
- 재실행 테스트: `Saved/Logs/CXTests.log`, AttackInputMapping / ServerRules 모두 Success, EXIT CODE 0.
- 과거 별도 서버+클라이언트2개 접속 검사는 정상 접속과 런타임 오류 부재를 확인한 범위이다. 채팅·상태·OwnerOnly·효과 동기화 전체를 검증한 것으로 확대 해석하지 않는다.
- 이번 요청은 검사이므로 게임 코드/사용자 애셋을 수정하지 않았다. 읽기 전용 감사 스크립트와 이 보고서를 추가했다.
