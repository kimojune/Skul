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
| `dt <초>` | 고정 프레임 시간 (기본값 1/60) |
| `hold <KEY> <from> <to>` | from~to 프레임 동안 키를 누름 (양끝 포함) |
| `press <KEY> <frame>` | 한 프레임만 누름 |
| `shot <frame>` | 해당 프레임을 그린 직후 화면 저장 |
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
| `player.state` | `Skul::eSkulState` 값: Idle 0, Move 1, Dash 2, Jump 3, Fall 4, Attack 5, Attack2 6, JumpAttack 7, SkillA 8, SkillS 9, ChangeSkill 10 |
| `count.<layer>` | 레이어의 활성(Active) 오브젝트 수. layer: `bg` `struct` `boss` `tile` `gate` `monster` `player` `bullet` `enemybullet` `ground` `test` `effect` `ui` |
| `objects` | 씬의 전체 오브젝트 수 (Pause·Death 포함, 누수 감시용) |

새 기능을 검사하려면 `yaScenario.cpp`의 `Scenario::collectState`에 변수를 추가하세요. 예를 들어 `player.hp`, `boss.hp`가 있습니다. 대본에 없는 변수를 쓰면 시작할 때 실패로 알려줍니다.

## 시나리오 작성 요령

- 기준선부터 잡으세요. 검사 없이 한 번 돌려 `state.jsonl`을 보고, 지금 동작하는 값을 확인한 다음 기대값을 정합니다.
- 기대값은 의도를 담는 정도로 느슨하게 잡으세요. 예를 들어 "점프하면 지면보다 150px 이상 올라간다"처럼 씁니다. 정확한 궤적 비교는 `state.jsonl` 두 개를 비교하는 쪽이 낫습니다.
- 몬스터 배회와 보스 패턴은 `rand()`에 의존합니다. 결과가 시드에 따라 달라지는 검사는 주석에 적어 두세요.
