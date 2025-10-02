# 🖥️ HappyBonkday – 언리얼엔진5

---

## 1. 프로젝트 개요

- **프로젝트명**: HappyBonkday
- **개발 기간**: 2025[.](http://yyyy.mm/)08.23 ~ 2025.10.01 (약 5주)
- **수행 방식**: 단독 개발
- **담당 범위**:  게임 플레이 시스템 및 캐릭터 전투 구현
- **깃허브 링크**
    - https://github.com/wodm15/HappyBonkday
- **기술 스택**: 언리얼엔진, C++
- **게임 시연 영상: https://youtu.be/aD4V3-cD8hY**

---

## 2. 게임 소개 및 구현 기능

### 🕹️ 게임 소개

> 소울라이크 전투 시스템을 차용하여 스테미나 기반 회피, 타격감 있는 공격
> 
> 
> 적을 처치하고 보물을 획득하여 일정 개수의 영혼과 골드를 획득하면 승리
> 

### 📌 요구사항 요약

- 캐릭터: 이동, 무기 장착/탈착, 회피, 공격, 특성, 죽음
- 적: 순찰, 추적, 공격, 소울 스폰, 죽음
- 환경: 파괴 가능한 물체, 파밍 가능한 보물 리스폰

---

## 3. 주요 기능

| 기능 | 설명 |
| --- | --- |
| 📱 메인 메뉴 | 게임 시작, 컴퓨터 사양 세팅, 개발자 크레딧 , 게임 종료 |
| 🪓 파괴 가능한 물체 및 보물 | 파괴 가능한 물체 존재, 파괴 시 랜덤 보물 리젠 |
| 🥋 기하학적 전투 시스템 | 공격 시에만 콜리션을 활성화, 위치 기반 애니메이션 실행 |
| 🥊 적 AI | 추적 및 공격, 죽음 처리, 모션 워핑으로 난이도 조절 |
| 🚴🏻 IK 시스템 | 발 위치 추적, 골반 오프셋 적용 |
| 🥋 캐릭터 기능 | 무기 장착/탈착, 숙여 걷기, 점프, 회피(스태미나 소모), 죽음 |

---

## 4. 문제 해결

### ✅ 문제 사례 1: 점프 후 떨어지는 도중 다시 점프와 달리기가 가능한 버그

→ 원인 : 착지 모션과 그라운드 모션 전환 조건 부족

→ 해결 방법: 착지 애니메이션 시간 0.25초, 그라운드 스피드 ≥ 0일 때만 실행

<img width="516" height="330" alt="image" src="https://github.com/user-attachments/assets/0d698d27-d210-4a9a-8551-38c14b80da8e" />
<img width="528" height="373" alt="image" src="https://github.com/user-attachments/assets/b67b574a-023d-4e84-b2f9-7bb584a892ac" />

### ✅ 문제 사례 2: 적에게 공격을 받고 체력이 0이되어도 한대 더 맞아야 죽는 버그

처음에는 GetHit_Implementation() 함수에서 죽음 판정. 하지만 언리얼의 ApplyDamage() 함수 호출 순서 때문에, 체력이 0이 되었을 때도 바로 죽지 않고 한번 더 맞아야되는 상황이 발생

→ 해결 ? : 죽음 판정을 GetHit_Implementation() 대신 TakeDamage() 함수 안에서 데미지를 반영한 직후 바로 죽음 여부를 판정하도록 수정

하지만  TakeDamage에서도 impactPoint 매개변수를 받아야하기 때문에 다른 방법 모색

→ 해결 방법 : pendingDamage 변수를 하나 추가하고 Damage를 미리 저장. GetHit_Implementation()에서 pendingDamage 데미지 값을 적용하고 살아있는지 확인.

<img width="883" height="457" alt="image" src="https://github.com/user-attachments/assets/d99c5afc-9082-4209-861e-bb564c7dfb66" />


### ✅ 문제 사례 3: 경사 있을 떄  다리가 떠있음

→ 해결 방법: 역운동학을 통해 다리와 땅 간의 추적을 통해 가상 왼발, 오른발의 오프셋을 추적.

오프셋 보간과 함께, 왼쪽 오른쪽 값중 무엇이 작은지 확인해서 골반의 오프셋 확인.

그 다음 오프셋을 활용해 IK 뼈들을 변형시킨다. (왼발, 오른발, 골반)

그리고 fullbody IK를 사용해서 iK 본들을 실제 본들의 위치에 적용 시킨다.

<img width="599" height="192" alt="image" src="https://github.com/user-attachments/assets/cba97444-4fea-4999-a7c9-6fcfd6730059" />

<img width="654" height="194" alt="image" src="https://github.com/user-attachments/assets/d05dc565-15ad-4814-8054-721f5b4f7808" />

<img width="589" height="495" alt="image" src="https://github.com/user-attachments/assets/9f31dc24-4422-45ed-9f0a-aae9ff1dc311" />


### ✅ 문제 사례 4: 공격 모션 후에 **다리가 다시 활처럼 구부러짐**

공격 모션 후 바로 leg IK를 실행하기 때문에 부자연스러운 다리 운동이 확인됨.

→ 해결 방법:  **iK 실행 전에 현실 발의 위치를 가져와서 현실발과 가상 발 위치 같도록 업데이트**

<img width="620" height="384" alt="image" src="https://github.com/user-attachments/assets/a4fa5882-f3b3-4bc0-a219-a8137635ff5e" />

### ✅ 문제 사례 5: 캐릭터 근처의 겹치는 아이템을 체크할 수 있는 Tick 최적화 문제

→ 해결 방법 : 프레임마다 호출하는 Tick 대신 델리게이트를 활용해서 캐릭터가 물체에 overlapped 될때를 상속해서 체크

item → IPickupInterface → 캐릭터가 IPickupInterface의 SetOverlappingItem 오버라이드

```jsx
Sphere->OnComponentBeginOverlap.AddDynamic(this, &AItem::OnSphereOverlap);
Sphere->OnComponentEndOverlap.AddDynamic(this , &AItem::OnSphereOverLapEnd);

```

---

## 5. 기술적 도전

### ✅ 기술적 도전 1. c++과 블루프린트를 동시에 사용 **

→ blueprintNative 이벤트 사용하여 c++ 를 거쳐 사운드만 블루프린트로 가도록 설계 

<img width="522" height="263" alt="image" src="https://github.com/user-attachments/assets/8b29ff22-95b7-40cf-81cb-d81914f6efea" />

### ✅ 기술적 도전 2. c++ 코드에서 일일히 세션이름을 Attack1, Attack2 처럼 직접 대입하는 하드 코딩 지양

→  c++ 코드가 아닌 인게임에서 변경되도록 수정

<img width="874" height="280" alt="image" src="https://github.com/user-attachments/assets/ef5f5c68-cf1a-488c-8b77-54c872bd8fc5" />

### ✅ 기술적 도전 3. Hit point로만 측정하고 있기 때문에 등에 뒤쪽으로 맞으면 적이 앞에 있어도 앞으로 넘어짐

→  Hit point가 아닌 공격자의 위치를 매개변수로 추가해서 Hit point 방향 변경 (내적과 외적 적용)

**충격 방향 계산**:

- 캐릭터의 전방 벡터와 충격 지점에서 캐릭터를 향한 벡터를 계산
- 두 벡터 간 내적으로 각도의 코사인 값을 구함
- 아크코사인을 적용하여 라디안을 구하고 이를 degree로 변환

**왼쪽/오른쪽 판별**:

- 두 벡터의 외적을 계산하여 충격이 왼쪽에서 왔는지 오른쪽에서 왔는지 판별
- 외적의 Z 값이 음수이면 왼쪽에서 온 충격으로 판단하고 각도에 -1을 곱함 (언리얼은 왼손 법칙)

<img width="610" height="554" alt="image" src="https://github.com/user-attachments/assets/f5c5058b-420e-4bab-bd4c-612b0546c1f4" />

 ### ✅ 기술적 도전 4. 숙여서 이동하는 crounch 기능 추가

→  캐릭터 타입에 스탠스스테이트를 추가하여 블랜드 포즈 분기를 통해 숙여서 이동하는 기능 추가

<img width="408" height="203" alt="image" src="https://github.com/user-attachments/assets/bf257348-a192-419d-8862-32c0df08f0d6" />

<img width="542" height="333" alt="image" src="https://github.com/user-attachments/assets/869bf455-a3c8-4854-a688-3152cf02c750" />


블랜더 무기 소유 여부 + 스탠스 상태에 따른 분기점

### ✅ 기술적 도전 5. 모션 워핑을 활용하여 적 난이도 조절

→ 위치와 회전을 분리하여 세밀하게 따라갈 수 있는 모션 워핑 구현 (모션 워핑 레일이 길수록 어려워짐)

<img width="707" height="693" alt="image" src="https://github.com/user-attachments/assets/a1e2ff0f-cf21-40fb-839d-24d86deae654" />

### ✅ 기술적 도전 6. 파괴 가능한 물체들

→ 지오메트리 컬렉션과 필드 시스템을 사용하여 무기를 휘두를 시 액터가 파괴되는 기능 구현

<img width="996" height="469" alt="image" src="https://github.com/user-attachments/assets/157c9bac-0a27-4b0f-a12a-a59111269754" />


### ✅ 기술적 도전 7. 향상된 Input 시스템 사용

→ 기존에 사용하던 입력 시스템 대신 입력 매핑 컨텍스트 사용

<img width="899" height="307" alt="image" src="https://github.com/user-attachments/assets/673d8955-29fd-4ff8-9e1a-1ceb6b9a3a84" />

### ✅ 기술적 도전 8. Link Anim 그래프 사용

→ 복잡하게 설계되어있던 IK와 MainState 애니메이션 블루프린트를 분리한 새로운 애니메이션 블루프린트 생성

<img width="384" height="267" alt="image" src="https://github.com/user-attachments/assets/4f0e2bdb-0d37-4fd6-83f1-a07445fe2d01" />

---

### ✅ 기술적 도전 9. 한손 무기, 두손 무기 장착에 따른 모션 변경

→ 한손 무기와 두손 무기를 따로 웨폰 타입을 구분하여 각기 다른 블렌드 스페이스를 적용

<img width="582" height="340" alt="image" src="https://github.com/user-attachments/assets/4ed6752e-b4b0-4342-9eb1-d66b71505707" />

<img width="964" height="653" alt="image" src="https://github.com/user-attachments/assets/a138c320-4b59-4b00-b730-f41ce713b4dd" />


### ✅ 기술적 도전 10. 적 애니메이션 블루프린트의 템플릿 사용

<img width="1423" height="466" alt="image" src="https://github.com/user-attachments/assets/8526f637-1206-4c93-92bb-ae16127a2d10" />

→ 여러 적들의 공통된 부분을 베이스로 삼아 베이스를 상속받은 애니메이션 블루프린트 사용

### ✅ 기술적 도전 11. 사운드 SFX 

<img width="826" height="453" alt="image" src="https://github.com/user-attachments/assets/2291445b-f692-4102-96bb-93e23bc7329a" />

→ 랜덤된 사운드, 랜덤 사운드 크기, 가상 서라운드 사운드 등을 대비하여 SFX 사운드를 활용

## 6. 개발 결과 및 회고

### 📈 결과

- 언리얼 엔진 5의 다양한 기능을 직접 구현하며 폭넓게 경험할 수 있었음
- 캐릭터 모션, IK 시스템, 몽타주 기반 전투 시스템, 파괴 가능한 환경 등 게임 시스템 구조를 설계하고 완성

### 🤔 회고

- 유니티보다 컴파일 시간이 길어 체감 개발 속도가 느림 → 블루프린트에서 프로토타입을 먼저 구현한 뒤 상속과 C++로 전환하는 전략이 효과적임
- 기술적으로 캐릭터 상태와 무기 장착/해제 로직, Montage 동기화 등 게임의 핵심 시스템을 안정적으로 구현
- 이번 프로젝트에서 많은 것을 배워갔으므로 다음 게임은 더 나아간 게임을 만들어볼 것이다.

---

---
