#include "yaCollisionManager.h"
#include "yaScene.h"
#include "yaSceneManager.h"

namespace ya
{
	WORD CollisionManager::mMatrix[(UINT)eLayerType::End] = {};

	std::map<UINT64, bool> CollisionManager::mCollisionMap;
	void CollisionManager::Update()
	{
		Scene* scene = SceneManager::GetActiveScene();

		for (UINT row = 0; row < (UINT)eLayerType::End; row++)
		{
			for (UINT col = 0; col < (UINT)eLayerType::End; col++)
			{
				if (mMatrix[row] & (1 << col))
				{
					LayerCollision(scene, (eLayerType)row, (eLayerType)col);
				}
			}
		}
	}

	void CollisionManager::LayerCollision(Scene* scene, eLayerType left, eLayerType right)
	{
		 std::vector<GameObject*>&  lefts = scene->GetGameObjects(left);
		 std::vector<GameObject*>& rights = scene->GetGameObjects(right);

		// 충돌 처리 중에 오브젝트가 생겨도(벡터 재할당) 안전하도록 인덱스로 돈다.
		// 새 오브젝트는 다음 프레임부터 충돌 검사를 받는다.
		const size_t leftCount = lefts.size();
		const size_t rightCount = rights.size();

		for (size_t i = 0; i < leftCount && i < lefts.size(); i++)
		{
			GameObject* leftObject = lefts[i];
			if (leftObject == nullptr)
				continue;

			Collider* leftCollider = leftObject->GetComponent<Collider>();
			if (leftCollider == nullptr)
				continue;

			for (size_t j = 0; j < rightCount && j < rights.size(); j++)
			{
				GameObject* rightObject = rights[j];
				if (rightObject == nullptr)
					continue;

				Collider* rightCollider = rightObject->GetComponent<Collider>();
				if (rightCollider == nullptr)
					continue;

				if (leftObject == rightObject)
					continue;

				// Pause·Death 오브젝트는 충돌하지 않는다.
				// 충돌 중이던 쌍이면 Exit를 한 번만 보내고 그 뒤로는 검사하지 않는다.
				if (leftObject->GetState() != GameObject::eState::Active
					|| rightObject->GetState() != GameObject::eState::Active)
				{
					InactiveCollision(leftCollider, rightCollider);
					continue;
				}

				ColliderCollision(leftCollider, rightCollider, left, right);
			}
		}
	}

	void CollisionManager::InactiveCollision(Collider* leftCol, Collider* rightCol)
	{
		ColliderID colliderID = {};
		colliderID.left = (UINT)leftCol->GetID();
		colliderID.right = (UINT)rightCol->GetID();

		std::map<UINT64, bool>::iterator iter
			= mCollisionMap.find(colliderID.id);

		if (iter == mCollisionMap.end() || iter->second == false)
			return;

		// 콜백 안에서 씬이 바뀌어(Clear) iter가 무효가 될 수 있으니 먼저 기록한다
		iter->second = false;

		leftCol->OnCollisionExit(rightCol);
		rightCol->OnCollisionExit(leftCol);
	}

	void CollisionManager::ColliderCollision(Collider* leftCol, Collider* rightCol, eLayerType left, eLayerType right)
	{
		ColliderID colliderID = {};
		colliderID.left = (UINT)leftCol->GetID();
		colliderID.right = (UINT)rightCol->GetID();

		//static std::map<UINT64, bool> mCollisionMap;
		std::map<UINT64, bool>::iterator iter
			= mCollisionMap.find(colliderID.id);

		if (iter == mCollisionMap.end())
		{
			mCollisionMap.insert(std::make_pair(colliderID.id, false));
			iter = mCollisionMap.find(colliderID.id);
		}

		if (Intersect(leftCol, rightCol))
		{
			// 최초 충돌 시작했을때 enter
			// (콜백 안에서 씬이 바뀌어 맵이 비워질 수 있으니 상태를 먼저 기록한다)
			if (iter->second == false)
			{
				iter->second = true;

				leftCol->OnCollisionEnter(rightCol);
				rightCol->OnCollisionEnter(leftCol);
			}
			else // 충돌 중인상태 stay
			{
				leftCol->OnCollisionStay(rightCol);
				rightCol->OnCollisionStay(leftCol);
			}
		}
		else
		{
			// Exit
			// 이전프레임 충돌 O
			// 현재는 충돌 X 
			if (iter->second == true)
			{
				iter->second = false;

				leftCol->OnCollisionExit(rightCol);
				rightCol->OnCollisionExit(leftCol);
			}
		}
	}
	bool CollisionManager::Intersect(Collider* left, Collider* right)
	{
		Vector2 leftPos = left->GetPos();
		Vector2 rightPos = right->GetPos();



		// 두 충돌체 간의 거리와, 각면적의 절반끼리의 합을 비교해서
		// 거리가 더 길다면 충돌 X, 거리가 더 짧다면 충돌 O
		Vector2 leftSize = left->GetSize();
		Vector2 rightSize = right->GetSize();

		leftPos.x = leftPos.x + leftSize.x / 2.0f;
		leftPos.y = leftPos.y + leftSize.y / 2.0f;

		rightPos.x = rightPos.x + rightSize.x / 2.0f;
		rightPos.y = rightPos.y + rightSize.y / 2.0f;


		if (fabs(leftPos.x - rightPos.x) < (leftSize.x / 2.0f) + (rightSize.x / 2.0f)
			&& fabs(leftPos.y - rightPos.y) < (leftSize.y / 2.0f) + (rightSize.y / 2.0f))
		{
			return true;
		}

		return false;
	}

	void CollisionManager::SetLayer(eLayerType left, eLayerType right, bool value)
	{
		UINT row = 0;
		UINT col = 0;

		UINT ileft = (UINT)left;
		UINT iright = (UINT)right;

		if (left <= right)
		{
			row = ileft;
			col = iright;
		}
		else
		{
			row = iright;
			col = ileft;
		}

		if (value == true)
			mMatrix[row] |= (1 << col);
		else
			mMatrix[row] &= ~(1 << col);
	}

	void CollisionManager::Clear()
	{
		memset(mMatrix, 0, sizeof(WORD) * (UINT)eLayerType::End);
		mCollisionMap.clear();
	}
}

