#pragma once
#include "yaStruct.h"

namespace ya
{

	class BackCastle : public Struct
	{
	public:
		BackCastle();
		~BackCastle();

		virtual void Initialize() override;
		virtual void Render(HDC hdc) override;


	private:
		Image* mImage;
	};

}