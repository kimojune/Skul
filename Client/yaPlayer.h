#pragma once
#include "yaGameObject.h"
#include "yaSkul.h"
namespace ya
{

	class Player : public GameObject
	{
	public:

		enum class eSkulType
		{
			Basic,
			NoHead,
			End,

		};
		
		Player();
		~Player();
		virtual void Initialize();
		virtual void Update();
		virtual void Render(HDC hdc);
		virtual void Release();

	private:
		std::vector<Skul*> mSkul;
		Skul* mActiveSkul;
	};

}
