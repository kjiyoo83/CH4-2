# CX 개발 목표 — Unreal Engine 5.6

사용자 요구사항 저장일: 2026-10-27. 사용자 구현 요청에 따라 C++ 및 애셋 구현을 진행했다. 아래 체크리스트는 원래 요구사항을 보존하며, 전체 실행 검증이 완료되기 전까지 완료 표시는 보류한다. 구현·실행 방법과 남은 화면 검증 항목은 `Docs/MULTIPLAYER_GUIDE.md`를 참고한다.

## 1. 채팅 시스템
- [ ] 채팅창 위젯 클래스 생성
- [ ] 채팅창 위젯 블루프린트 클래스 애셋 생성
- [ ] 위젯 객체 생성 및 연결
- [ ] 채팅창 엔터 이벤트 로직 작성
- [ ] 작성한 메시지를 화면에 표시
- [ ] 멀티플레이 채팅 동기화

## 2. PlayerState 및 개인 UI
- [ ] Coins 속성 및 프로퍼티 레플리케이션 구현
- [ ] Coins 증가 및 차감 함수 정의
- [ ] Coins는 서버에서만 변경되도록 검사
- [ ] Coins를 DOREPLIFETIME_CONDITION의 COND_OwnerOnly로 복제
- [ ] BonusMaxSpeed 속성 및 프로퍼티 레플리케이션 구현
- [ ] BonusMaxSpeed 증가 함수 정의
- [ ] BonusMaxSpeed는 서버에서만 변경되도록 검사
- [ ] BonusMaxSpeed를 DOREPLIFETIME_CONDITION의 COND_OwnerOnly로 복제
- [ ] 자신의 Coins와 BonusMaxSpeed를 자신의 화면에 표시하는 UI 제작 및 연결

## 3. 차량 공격, 아이템 및 스포너
- [ ] 공격용 좌클릭 InputAction 애셋 생성 및 차량 폰 연결
- [ ] 좌클릭 시 차량 폰의 Server RPC 호출
- [ ] Server RPC에서 앞 범퍼 중앙을 시작점으로 라인트레이스
- [ ] 상대 차량 감지 시 ApplyDamage() 호출
- [ ] 피해 차량 Coins가 5 이상이면 가해 차량에 5원 추가하고 피해 차량에서 5원 차감
- [ ] 피해 차량 Coins가 5 미만이면 코인 이전 없음
- [ ] DrawDebugLine()으로 라인트레이스 표시 및 피격 차량 파티클 재생
- [ ] 코인 액터 생성: 오버랩 플레이어 Coins +10
- [ ] 코인 액터 NetUpdateFrequency = 1
- [ ] 획득한 코인 액터 파괴 및 파티클/사운드 재생
- [ ] 스피드 버프 액터 생성: 오버랩 플레이어 BonusMaxSpeed +10
- [ ] 스피드 버프 액터 NetUpdateFrequency = 1
- [ ] 획득한 버프 액터 파괴 및 파티클/사운드 재생
- [ ] 모든 플레이어에게 동일한 아이템 상태와 획득 결과 동기화
- [ ] 스포너 액터 클래스와 액터 스폰 로직 구현
- [ ] 스포너가 생성한 아이템 참조 저장 및 OnDestroyed 델리게이트 구독
- [ ] 생성 아이템 파괴 2초 뒤 같은 종류 아이템 리스폰
- [ ] 레벨에 스포너 배치 및 스포너별 코인/버프 종류 설정

## 4. GameMode, GameState 및 경기 UI
- [ ] GameState 클래스 생성
- [ ] Waiting, Playing, Finished 상태 Enum 정의
- [ ] GameState에 현재 상태와 남은 시간 속성 정의 및 동기화
- [ ] GameState에 우승자 목록 속성 정의 및 동기화
- [ ] 게임 시작 시 Waiting 설정
- [ ] 첫 플레이어 접속 시 대기 카운트다운 시작
- [ ] 서버 타이머가 1초마다 대기 시간을 감소시키고 UI 표시
- [ ] 대기 종료 시 Playing 전환
- [ ] 서버 타이머가 1초마다 플레이 시간을 감소시키고 UI 표시
- [ ] 플레이 종료 시 Finished 전환
- [ ] 모든 플레이어 Coins를 비교하여 최고 보유자 탐색
- [ ] 최고 Coins 동점자는 모두 우승자 목록에 포함
- [ ] 로컬 플레이어의 승리/패배 결과 UI 구현

## 구현 시 확정할 세부 사항
- 기본값: 대기 10초, 경기 120초. GameMode 블루프린트에서 변경 가능.
- 기본값: 공격 거리 800cm, 공격 간격 0.5초. 차량 블루프린트에서 변경 가능.
- BonusMaxSpeed 단위: km/h. 기본 수평 속도 제한 120km/h에 가산.
- 기본 UI와 StarterContent 폭발 효과/사운드 사용. VehicleAdvExampleMap에 스포너 9개 배치.

## 현재 검증 상태
- 2026-10-27 순서별 재검사: 원래 사용자 체크박스 47개 중 요구사항 수준 확인 36개, 구현은 있으나 추가 실행 확인 필요 11개. 상세 판정과 근거: `Docs/CHECKLIST_AUDIT.md`.
- C++ 기능 및 요구 애셋 생성/연결 완료, UE 5.6 에디터 빌드 성공.
- CX.Gameplay.ServerRules 자동 테스트 성공.
- 별도 서버/클라이언트 2개 접속 검사 성공.
- 전체 체크리스트의 실행 완료 판정은 Docs/MULTIPLAYER_GUIDE.md의 두 클라이언트 화면 검증 후 확정.
