#pragma once

#include "Book.hpp"
#include "PoolManager.hpp"


class Player;

class BookManager : public PoolManager<Book>
{
public:
	void SpawnBatch(int count);
	void Init(const Player *player);

private:
	const Player* _player = nullptr;
	int _textureIndex = 0;
};