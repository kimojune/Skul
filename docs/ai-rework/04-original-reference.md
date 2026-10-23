# 04. 원작 참고 자료 (Skul: The Hero Slayer 1장)

원작 「Skul: The Hero Slayer」의 1장 기준 동작을 정리한 문서입니다. Phase 2부터 게임플레이를 구현할 때 기준으로 씁니다.
Phase 0~1은 엔진 작업이라 원작을 따로 조사하지 않았습니다. 게임플레이에 들어가기 전에 이 문서를 만들었습니다(2026-10-23, 조사 서브에이전트).

- 각 항목에는 **확인됨**(출처에 적혀 있음) 또는 **불확실**(추정, 또는 출처끼리 다름)을 붙였습니다.
- "클론 반영" 열은 저장소에 있는 리소스만으로 구현할 수 있는지를 뜻합니다.

## 리틀 본 (기본 스컬)

| 항목 | 원작 | 근거 | 클론 반영 |
|---|---|---|---|
| 기본 공격 | 내려치기 → 백핸드 2타 반복. 공중 공격 1종. 대시 공격 없음 | 확인됨 [F1][N2] | 이미 구현 |
| 점프 | 2단 점프 | 확인됨 [N1] | 무한 점프 버그를 2단으로 제한 |
| 대시 | 2회 충전. 대시 중 무적이고, 맞은 투사체는 사라짐 | 확인됨 [N1][N2]. 쿨다운 수치는 불확실 | 구현 |
| A 두개골 투척 | 머리를 던져 마법 피해. 쿨다운 6초. 머리를 회수하면 쿨다운 초기화. 공중에서는 1회만 투척. 머리는 무언가에 부딪히거나 시간이 지나면 위로 튐 | 확인됨 [F1][N2]. 옛 가이드는 10초 [SA] | 8초 자동 복귀를 원작 방식으로 변경 |
| S 순간이동 | 머리가 떨어져 있을 때만 머리 위치로 이동. 쿨다운 3초 | 확인됨 [F1][N2]. 이동하면 머리도 회수되는지는 불확실 | 구현 |
| 교대 (Space) | 두 스컬을 맞바꾸고 교대 스킬 발동. 대기 중인 스컬도 쿨다운이 흐름 | 확인됨 [F1][N2] | 두 번째 스컬 리소스가 없어 보류 |

## 생존

| 항목 | 원작 | 근거 |
|---|---|---|
| 최대 HP | 100 (영구 강화로 최대 150) | 확인됨 [N1][SS] |
| 받는 피해 | 1장 큰 엔트 10 이상, 중간보스 단발 10~20 | 확인됨 [N1] |
| 피격 무적 | **없음**. 다단히트를 연달아 맞음. "피격 시 1초 무적"은 별도 능력임 | 확인됨 [N1][F5] |
| 넉백 | 출처 없음 | 불확실 |
| 사망 | 아이템·정수·스컬을 잃고 마석만 남긴 채 처음부터 다시 시작 | 확인됨 [GS][G] |

## HUD

| 위치 | 원작 | 근거 |
|---|---|---|
| 좌하단 | 현재 스컬 큰 초상화, 보조 스컬 작은 초상화(SPACE 표기), A·S 스킬 아이콘(남은 쿨다운 초 표시), D 정수 칸, **HP 바 + "59 / 105" 숫자** | 확인됨 [SS][G] |
| 우하단 | 미니맵(적은 빨간 점), 남은 적 수, 골드·마석 개수 | 확인됨 [SS][G][N3] |
| 상단 중앙 | 보스 바. 붉은 바 위에 이름 "Yggdrassil"(게임 내 철자), 아래에 칭호 "Elder Ent". 수치 표시 없음 | 확인됨 [SS] |

## 1장 적

1장 이름은 "조화의 숲"이고, Deep Woods → Carleon Border → Carleon Gate 순서입니다 [F3][N3].

| 적 | 공격 | 저장소 리소스 |
|---|---|---|
| 엔트 | 주먹을 들었다 내려침. 느림 | 대기·이동·피격 프레임만 있음 (공격 프레임 없음) |
| 꽃 엔트 | 슈퍼아머로 예비동작한 뒤 주변에 꽃가루 구름(도트 피해) | 구름 이펙트만 있고 본체 없음 |
| 뿌리 엔트 | 플레이어 발밑에 범위를 표시한 뒤 가시 장판 | 없음 |
| 숲지기 | 움직이지 않음. 멀면 6방향 구체, 가까우면 양손 충격파 | 없음 |
| 칼레온 신병·궁수·중보병·암살자·마도사 | 근접, 화살, 돌진, 순간이동 베기, 화염구, 고드름 | 없음 |

## 보스: 장로 엔트 위그드라실

HP는 1페이즈 1080 + 2페이즈 1080입니다 [F2][N4].

| 패턴 | 1페이즈 | 2페이즈 |
|---|---|---|
| 주먹 | 두 주먹을 들고 떨다가 가까운 쪽 주먹으로 플레이어 위치를 내려침. 2회. 내려친 손은 잠시 발판이 됨 | 2+2회, 더 빠름 |
| 쓸기 | 손을 화면 밖으로 벌리고 기울어진 뒤 약 1초 후 화면이 흔들리며 바닥을 쓸어냄. 2회 | 좌우 2+2회 |
| 에너지볼 | 이마 마석에 2초 충전한 뒤 조준 큰 구체 + 8방향 작은 구체. 3회 반복 후 쓰러져 **그로기** | 1초 충전 뒤 큰 구체가 화면 곳곳에 쏟아짐(약한 유도). 끝나면 그로기 |
| 양손 3연타 | — | 바닥이 붉게 빛나 예고한 뒤 3연타. 세 번째는 늦게 들어옴 |
| 피격 | 그로기가 아닐 때는 머리만 맞출 수 있음 | 같음 |
| 페이즈 전환 | — | 몸이 보라색 톤으로 바뀌고 바닥의 뿌리 발판이 사라짐 |
| 연출 | 등장 시 땅을 움켜쥠. 사망 시 마석이 빠지며 제정신을 찾고 스컬에게 감사 | |

저장소에 있는 대응 리소스는 다음과 같습니다. 이름과 패턴의 대응은 추정입니다. [추론]

- 등장: `Appearance Impact`, `Appearance.wav`
- 그로기: `Groggy` 사운드
- 양손 3연타: `Roar_BothFistPowerSlam`, `Boss/Effect/smoke`
- 사망: `smoke_origin`(Golem_Explosion), `Dead_Impact`, `Dead_Roar`
- 보스 BGM: `Chapter1_Boss.wav`

## 1장 흐름

- 방의 적을 모두 처치해야 문이 열립니다 [F3][N1].
- 문 종류 [N1][N2][F3]:
  - 주황: 골드
  - 노랑: 아이템
  - 초록: 스컬
  - 빨강: 모험가 중간보스
  - 그 밖에 상점, 다음 층, 보스 문
- 보스를 처치하면 보상 상자("Elder Ent's Gratitude"), HP 결정, 각성 NPC 아라크네가 나오고 2장으로 넘어갑니다 [F5][N1][N3].

## 클론과 원작의 주요 차이 (Phase 2 판단 근거)

1. 머리 8초 자동 복귀: 원작은 쿨다운 6초이고 직접 회수합니다. 순간이동은 별도 3초 쿨다운이고, 공중 투척은 1회입니다.
2. 대시 2회 충전과 무적, 2단 점프 제한이 없습니다. 지금은 무한 점프·대시 버그가 있습니다.
3. HP 100과 사망 처리, HUD, 보스 바가 없습니다. 원작에는 피격 무적이 없다는 점도 반영해야 합니다.
4. 보스에 그로기, 2페이즈, 양손 3연타, 머리 한정 피격, 조준 큰 구체, 등장·사망 연출이 빠져 있습니다.
5. 적은 엔트 1종뿐입니다. 다른 적과 문 종류는 리소스가 없어 이 저장소만으로는 만들 수 없습니다.

## 출처

| 키 | 출처 |
|---|---|
| N1 | [나무위키 – Skul: The Hero Slayer](https://namu.wiki/w/Skul:%20The%20Hero%20Slayer) |
| N2 | [나무위키 – 스컬](https://namu.wiki/w/Skul:%20The%20Hero%20Slayer/스컬) |
| N3 | [나무위키 – 스테이지 및 적](https://namu.wiki/w/Skul:%20The%20Hero%20Slayer/스테이지%20및%20적) |
| N4 | [나무위키 – 모험가 및 보스](https://namu.wiki/w/Skul:%20The%20Hero%20Slayer/모험가%20및%20보스) |
| F1 | [Skul Wiki – Skul](https://skul.fandom.com/wiki/Skul) |
| F2 | [Skul Wiki – Elder Ent: Yggdrasil](https://skul.fandom.com/wiki/Elder_Ent:_Yggdrasil) |
| F3 | [Skul Wiki – Gates](https://skul.fandom.com/wiki/Gates), [Stages](https://skul.fandom.com/wiki/Stages) |
| F4 | [Skul Wiki – Enemies](https://skul.fandom.com/wiki/Enemies) |
| F5 | [Skul Wiki – Witch](https://skul.fandom.com/wiki/Witch), [Dark Ability](https://skul.fandom.com/wiki/Dark_Ability), [Update History](https://skul.fandom.com/wiki/Update_History), [Elder Ent's Gratitude](https://skul.fandom.com/wiki/Elder_Ent's_Gratitude) |
| T | [Twinfinite – Yggdrasil 공략 (보관본)](http://web.archive.org/web/20210620070825/https://twinfinite.net/2021/01/skul-beat-yggdrasil-fight/) |
| G | [GIGAZINE 리뷰](https://gigazine.net/gsc_news/en/20210502-skul-the-hero-slayer) |
| GS | [GameSpace 리뷰](https://gamespace.com/featured/our-review-of-skul-the-hero-slayer/) |
| SA | [SteamAH – 스컬 목록](https://steamah.com/skul-the-hero-slayer-all-skulls-list/) |
| SS | [Steam 스토어 스크린샷](https://store.steampowered.com/app/1147560) |
