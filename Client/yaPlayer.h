#pragma once
#include "yaGameObject.h"
#include "yaSkul.h"

namespace ya
{

	class Player : public GameObject
	{
	public:

		Player();
		~Player();

		enum class eSkulType
		{
			Basic,
			Nohead,
			End,
		};

		virtual void Initialize() override;
		virtual void Update() override;
		virtual void Render(HDC hdc) override;
		virtual void Release() override;

	private:
		std::vector<Skul*> mSkul;
		Skul* mActiveSkul;
	};
}
