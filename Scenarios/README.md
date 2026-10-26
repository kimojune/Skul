# 시나리오 테스트

게임을 사람 대신 **입력 대본**으로 플레이하고, 매 프레임 상태를 기록하며 기대값을 검사합니다.
창과 소리 없이, 프레임 시간을 1/60초로 고정해서 돌리므로 같은 대본은 항상 같은 결과를 냅니다.
실제 시간보다 빠르게 돕니다.

```
cd Client
<빌드 폴더>\Client.exe --scenario ..\Scenarios\stage1_land.txt <출력 폴더>
```

- Resources를 `..\Resources`로 읽으므로 **Client 폴더에서 실행**해야 합니다.
- 종료 코드: `0` 통과, `1` 검사 실패, `2` 대본 문법 오류. 크래시는 그 밖의 코드로 끝납니다.
- 출력 폴더:
  - `result.txt`: 통과/실패 요약
  - `state.jsonl`: 프레임마다 상태 한 줄
  - `shot_<프레임>.bmp`: 스크린샷

## 대본 문법

한 줄에 명령 하나를 씁니다. `#` 뒤는 주석입니다. 프레임 번호는 0부터 셉니다.

| 명령 | 뜻 |
|---|---|
| `scene <Title\|Stage1\|Stage2\|Boss\|Tool>` | 시작 씬 (기본값 Title) |
| `frames <n>` | 총 실행 프레임 수 (기본값 600) |
| `seed <n>` | `rand()` 시드 (기본값 1). 몬스터 배회와 보스 패턴 선택이 바뀝니다 |
| `dt <초>` | 고정 프레임 시간 (기본값 1/60). 엔진이 프레임 시간을 1/30초로 제한하므로 그보다 큰 값은 1/30으로 처리됩니다 |
| `hold <KEY> <from> <to>` | from~to 프레임 동안 키를 누름 (양끝 포함) |
| `press <KEY> <frame>` | 한 프레임만 누름 |
| `shot <frame>` | 해당 프레임을 그린 직후 화면 저장 |
| `teleport <frame> <x> <y>` | 해당 프레임 시작에 플레이어를 (x, y)로 옮김 (속도 0, 공중). 맵 여러 지점을 한 대본으로 훑을 때 사용 |
| `timefx <on\|off>` | `off`면 히트스톱·슬로모션의 시간 배율만 끈다 (소리·흔들림·번쩍임은 그대로). 히트스톱이 프레임 타이밍을 밀어 기대값이 어긋나는 대본에서 쓴다. 기본값 on |
| `fxknock <on\|off>` | `off`면 엔트 넉백·경직을 예전 값(약 18px, 0.3초)으로 되돌리고 공격 돌진을 끈다. 20프레임 간격으로 연타하는 대본이 닿도록. 기본값 on |
| `damage <frame> <amount>` | 해당 프레임 시작에 플레이어에게 피해 (`PlayerStats::TakeDamage`). 대시 무적 중이면 무시됨 |
| `expect <frame> <var> <op> <number>` | 해당 프레임이 끝난 뒤 검사 |
| `always <var> <op> <number>` | 매 프레임 검사. 처음 실패한 프레임을 보고 |

- `op`: `<` `<=` `>` `>=` `==` `!=`
- 위치나 속도가 NaN 또는 무한대가 되면 대본과 관계없이 바로 실패합니다.
- KEY: `Q`~`M`(알파벳), `UP` `DOWN` `LEFT` `RIGHT` `LBUTTON` `RBUTTON`, `SPACE` `SHIFT` `ESC` `ENTER` `TAB`, `1` `2` `3` `4`(키보드 윗줄 숫자). 이 목록은 `yaInput.cpp`의 `KeyNames`와 같습니다.
- 게임 조작:
  - ←→: 이동
  - C: 점프
  - Z: 대시
  - X: 공격
  - A, S: 스킬
  - ↓: 발판 아래로 내려가기
  - F: 게이트

## 상태 변수

| 변수 | 뜻 |
|---|---|
| `frame`, `scene` | 프레임 번호, 현재 씬. scene 값: Title 0, Stage1 1, Stage2 2, Stage3 3, Boss 4, Ending 5, Tool 6 |
| `player.found` | 플레이 씬에서 조작 중인 스컬이 있으면 1 |
| `player.x`, `player.y` | 위치. y는 아래로 갈수록 커짐. Stage 지면이 y=1726 |
| `player.vx`, `player.vy` | Rigidbody 속도. 걷기는 Transform을 직접 옮기므로 vx가 0일 수 있습니다 |
| `player.ground` | 착지 상태(1/0) |
| `player.state` | `Skul::eSkulState` 값: Idle 0, Move 1, Dash 2, Jump 3, Fall 4, Attack 5, Attack2 6, JumpAttack 7, SkillA 8, SkillS 9, ChangeSkill 10, Fall_Dead 11(사망) |
| `count.<layer>` | 레이어의 활성(Active) 오브젝트 수. layer: `bg` `struct` `boss` `tile` `gate` `monster` `player` `bullet` `enemybullet` `ground` `test` `effect` `ui` |
| `objects` | 씬의 전체 오브젝트 수 (Pause·Death 포함, 누수 감시용) |
| `player.hp`, `player.maxhp`, `player.invincible` | `PlayerStats`의 HP, 최대 HP, 무적 여부(대시 중 등) |
| `player.jumpCount`, `player.dashCharges`, `player.headOut`, `player.skillACooldown`, `player.skillSCooldown` | `PlayerStats`가 내보내는 이동·스킬 값: 이번 공중에서 쓴 점프 수(최대 2), 남은 대시 충전(최대 2), 머리를 던졌는지(1/0), A·S 남은 쿨다운(초) |
| `head.state`, `head.x`, `head.y` | 던진 머리(`SkulHead`): 0 몸에 붙음, 1 날아감, 2 튀어 떨어짐, 3 바닥에 놓임. 위치는 머리 Transform (그림 아래 끝 = y − 14) |
| `boss.hp` | 1장 보스(`Chapter1_Boss`) HP. 1페이즈 40 + 2페이즈 40 = 80에서 시작. 플레이어 공격 한 대에 1 |
| `boss.phase` | 1 또는 2. HP가 40이 되면 2 |
| `boss.state` | `Chapter1_Boss::eBossState` 값: Intro 0, Idle 1, Punch 2, Sweep 3, EnergyBall 4, Groggy 5, PhaseChange 6, EnergyRain 7, TripleSlam 8, Dead 9 |
| `boss.groggy` | 그로기 중이면 1. 이때는 머리 말고 몸통·손도 맞는다 (state가 5로 바뀐 다음 프레임부터) |
| `boss.volleys` | 이번 에너지볼 패턴에서 쏜 횟수 (3이면 그로기로 넘어감). 다음 에너지볼 패턴이 시작될 때 0 |
| 그 밖의 이름 | 게임 코드가 `Scenario::Publish("이름", 값)`으로 내보낸 값 (예: `boss.hp`). Initialize에서 초깃값을 내보내야 대본 시작 검사를 통과합니다 |
| `fx.timescale` | 지금 시간 배율. 히트스톱 중 0, 슬로모션 중 0.3, 평소 1 (`timefx off`면 항상 1) |
| `fx.hitstops` | 히트스톱이 걸린 누적 횟수 (적을 때릴 때, 플레이어가 맞을 때) |
| `fx.flashing` | 지금 흰색으로 번쩍이는 오브젝트(Animator) 수 |
| `fx.slowmo` | 슬로모션이 걸린 누적 횟수 (마지막 엔트 처치, 보스 페이즈 전환, 보스 사망) |
| `camera.x`, `camera.y` | 화면 가운데가 보는 월드 좌표. 화면 왼쪽 위는 (x−800, y−450). 플레이 씬의 카메라 경계는 x 800~2550 |

새 기능을 검사하려면 `yaScenario.cpp`의 `Scenario::collectState`에 변수를 추가하세요. 예를 들어 `player.hp`, `boss.hp`가 있습니다. 대본에 없는 변수를 쓰면 시작할 때 실패로 알려줍니다.

## 시나리오 작성 요령

- 기준선부터 잡으세요. 검사 없이 한 번 돌려 `state.jsonl`을 보고, 지금 동작하는 값을 확인한 다음 기대값을 정합니다.
- 기대값은 의도를 담는 정도로 느슨하게 잡으세요. 예를 들어 "점프하면 지면보다 150px 이상 올라간다"처럼 씁니다. 정확한 궤적 비교는 `state.jsonl` 두 개를 비교하는 쪽이 낫습니다.
- 몬스터 배회와 보스 패턴은 `rand()`에 의존합니다. 결과가 시드에 따라 달라지는 검사는 주석에 적어 두세요.
