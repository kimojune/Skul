#pragma once


enum class eSceneType
{
	Title,
	Play,
	Play2,
	Ending,
	Tool,
	End,
};

enum class eLayerType
{
	BG,
	Struct,
	Tile,
	Monster,
	Player,
	Bullet,
	Ground,
	Test,
	Effect,
	UI,
	End = 16,
};

enum class eComponentType
{
	Transform,
	SpriteRenderer,
	Animator,
	Collider,
	Rigidbody,
	Audio,
	End,
};

enum class eDirection
{
	Left,
	Right,
	Up,
	Down,
	LeftUp,
	RightUp,
	LeftDown,
	RightDown,
	End,
};