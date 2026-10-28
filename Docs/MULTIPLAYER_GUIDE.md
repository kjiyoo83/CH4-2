# CX 멀티플레이 과제 구현 및 실행 안내

## 실행
1. UE 5.6으로 `CX.uproject`를 연다. 이미 열려 있었다면 종료 후 다시 연다.
2. `/Game/VehicleTemplate/Maps/VehicleAdvExampleMap`을 연다.
3. Play 버튼 옆 메뉴에서 Number of Players를 2로 설정한다.
4. Net Mode를 Play As Client로 설정한다. Editor Preferences의 Multiplayer Options에서 별도 서버를 사용하고 Run Under One Process를 끄면 프로세스 간 동기화를 확인할 수 있다.
5. New Editor Window (PIE)로 실행한다. 첫 접속 후 대기 10초, 경기 120초가 진행된다.

## 조작 및 규칙
- Enter: 채팅 입력. 다시 Enter: 전송하고 운전으로 복귀. Escape: 입력 취소.
- 좌클릭: 전방 800cm 공격. 서버가 0.5초 간격을 검사한다.
- 피해자가 5원 이상 가진 경우에만 피해자 -5원, 공격자 +5원.
- 금색 코인 획득: +10원. 파란 큐브 획득: 추가 최대 속도 +10km/h.
- 자신의 원화와 속도 보너스, 공통 경기 상태 및 남은 시간은 왼쪽 위에 표시된다.
- 경기가 끝나면 보유 원화가 최대인 모든 플레이어가 승리한다. 전원 0원도 공동 우승이다.
- 공격과 아이템 획득은 Playing 상태에서만 허용한다. 종료 후 결과를 유지한다.
- 기본 속도 제한은 120km/h이며 보너스만큼 제한이 높아진다. 수평 물리 속도를 서버와 소유 차량에서 제한한다. 엔진 출력·기어비에 따른 실제 도달 속도와는 구별된다.

## 생성·연결한 애셋
`/Game/CXMultiplayer`:
- `WBP_Chat`: UChatUI를 상속하는 위젯 블루프린트. 빈 WidgetTree에서 C++ 기본 채팅/HUD 레이아웃을 생성한다.
- `IA_Attack`: Boolean InputAction. 기존 IMC_Default에 LeftMouseButton으로 연결한다.
- `BP_Coin`, `BP_SpeedBuff`, `BP_ItemSpawner`.

기존 스포츠카·오프로드 차량 블루프린트의 AttackAction, 플레이어 컨트롤러의 ChatUIClass, 두 GameMode의 PlayerStateClass/GameStateClass를 연결했다.
기본 VehicleAdvExampleMap에 코인 스포너 6개, 속도 버프 스포너 3개를 PlayerStart 주변에 저장했다. 오프로드 예제 맵에는 배치하지 않았다.

## 조정할 수 있는 값
- GameMode: WaitingDuration=10, PlayingDuration=120.
- 차량: AttackOrigin 위치(기본 로컬 X=220, Y=0, Z=40), AttackRange=800cm, AttackCooldown=0.5초, BaseMaxSpeed=120km/h, HitEffect.
- 아이템: PickupEffect, PickupSound. 기본 StarterContent 폭발 파티클/사운드를 사용한다.
- 스포너: ItemClass, RespawnDelay=2초. 인스턴스별로 종류와 위치를 조정할 수 있다.

## 구조
- `ACXPlayerState`: 서버 권한 검사, Coins/BonusMaxSpeed의 COND_OwnerOnly 복제.
- `ACXPlayerController`: 소유 클라이언트 UI 생성, 채팅 Server RPC → 서버에서 각 PlayerController의 Client RPC 호출. 메시지 최대 256자, 서버 최소 간격 0.3초, 표시 기록 100줄.
- `ACXPawn`: 서버 라인트레이스/ApplyDamage/코인 이전, 공격 표시 multicast, 차량 속도 제한.
- `ACXPickup`, `ACXCoin`, `ACXSpeedBuff`: 서버 획득 판정, NetUpdateFrequency=1, 효과 multicast 후 서버 파괴.
- `ACXItemSpawner`: 서버 스폰, 아이템 참조와 OnDestroyed 구독, 2초 타이머 리스폰, 레벨 종료 시 정리.
- `ACXGameMode`: 최초 로그인 타이머 시작과 Waiting→Playing→Finished 전환 및 서버 우승 판정.
- `ACXGameState`: 상태/남은 시간/공동 우승자 목록 복제.

## 확인해야 할 플레이 항목
아래 항목은 실제 두 클라이언트 창에서 화면과 사운드를 확인해야 한다. 단일 월드 자동 테스트는 네트워크 전달이나 화면 품질까지 증명하지 않는다.
- [ ] 양쪽에서 Enter로 보낸 채팅이 두 화면에 한 번씩 표시된다.
- [ ] 채팅 중 가속 입력이 남지 않고 Enter/Escape로 운전 복귀한다.
- [ ] 코인 획득 시 자신의 HUD만 +10, 두 화면 모두에서 해당 코인이 사라지고 2초 후 다시 보인다.
- [ ] 버프 획득 시 자신의 Bonus만 +10, 아이템 효과와 사운드가 두 화면에서 재생된다.
- [ ] 상대 앞에서 공격하면 선/피격 효과가 보이며 코인이 정확히 5원 이전된다.
- [ ] 피해자 잔액 0~4원에서는 양쪽 잔액이 변하지 않는다.
- [ ] 두 창의 경기 상태와 남은 시간이 일치한다. 네트워크 전송 지연에 따른 짧은 표시 지연은 있을 수 있다.
- [ ] 단독/공동 우승 및 패배 화면이 맞고 종료 후 획득·공격으로 잔액이 변하지 않는다.
- [ ] 월드 아웃라이너의 `CX_ItemSpawner_*`가 도로 위에 보이고 차량으로 접근할 수 있는 위치인지 확인한다.

## 재생성 및 테스트
좌클릭 입력이 작동하지 않으면 `IMC_Default`에 `IA_Attack → LeftMouseButton` 매핑이 실제 저장되어 있는지 확인한다. `Scripts/repair_attack_mapping.py`는 정리된 Input 폴더에서도 기존 IA_Attack을 찾아 누락 매핑만 추가하고 강제로 저장한다. 외부 명령줄 에디터에서 실행할 때는 파일 잠금을 피하도록 작업 중인 에디터/게임을 먼저 저장하고 종료한다.

`Scripts/setup_multiplayer.py`는 UE PythonScriptPlugin을 사용하는 반복 실행 가능한 애셋 설정 스크립트다. 이미 생성된 애셋과 같은 라벨의 스포너는 재사용한다. 수동 편집 후에는 필요할 때만 실행한다.

C++ 자동 테스트 `CX.Gameplay.ServerRules`는 서버 권한, 코인 부족/이전, 실제 공격 트레이스, 아이템 오버랩, 리스폰 타이밍, 경기 전환, 동점 우승, 종료 후 공격 무효를 검사한다.
검증 로그: `Saved/Logs/CXSetup.log`, `Saved/Logs/CXTests.log`.

### 이번 작업에서 확인한 결과
- UE 5.6 `CXEditor Win64 Development` C++ 빌드 성공.
- 애셋 설정 스크립트: 오류 0, 경고 0으로 완료. 기본 맵 스포너 9개 저장 확인.
- `CX.Gameplay.ServerRules`: Success, 종료 코드 0. 서버 권한 검사, 실제 라인트레이스 공격, 코인 부족 분기, 코인/버프 실제 오버랩, 리스폰, 상태 전환, 공동 우승 검증.
- `Scripts/network_smoke.ps1`: 별도 서버 + 별도 클라이언트 2개 접속 성공. Fatal/Ensure/스크립트/RPC 오류 없음. 테스트로 실행한 프로세스는 자동 종료.
- 네트워크 접속 검사는 NullRHI로 실행했다. 실제 채팅 입력 왕복, OwnerOnly 값의 타 클라이언트 비노출, 프레임별 타이머 표시, 효과/사운드, 최종 UI 렌더링은 위 수동 확인 목록으로 검증해야 한다.

멀티플레이 최종 화면 검증이 끝나기 전에는 전체 과제의 실행 검증 완료로 간주하지 않는다.
