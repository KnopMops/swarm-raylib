#include "BookManager.hpp"

#include "Player.hpp"
#include "ResourceKeys.hpp"
#include "SwarmUtils.hpp"

#include "array"


constexpr std::array<const char *, 4> BOOK_TEXTURES = {
	RK::BOOK_1, RK::BOOK_2, RK::BOOK_3, RK::BOOK_4
};

void BookManager::SpawnBatch(int count)
{
	for (int i = 0; i < count; i++)
	{
		auto* book = spawnInPool();
		const char* textureName = BOOK_TEXTURES[_textureIndex];
		_textureIndex = (_textureIndex + 1) % (int)BOOK_TEXTURES.size();
		book->Activate(RandomSpawnPoint(_player->GetPosition(), 150.0f), textureName);
	}
}

void BookManager::Init(const Player *player)
{
	_player = player;
}