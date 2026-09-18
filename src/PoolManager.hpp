#pragma once

#include "PoolObject.hpp"

#include "vector"
#include "memory"
#include "type_traits"


template <typename T>
class PoolManager
{
	static_assert(std::is_base_of_v<PoolObject, T>, "T is not PoolObject");
public:
	virtual void Update(float dt)
	{
		for (const auto& object : _pool) object->Update(dt);
	}

	virtual void Draw()
	{
		for (const auto& object : _pool) object->Draw();
	}

	virtual void DeactivateAll()
	{
		for (const auto& object : _pool)
			if (object->IsAlive()) object->Deactivate();
	}

	const std::vector<std::unique_ptr<T>>& GetPool() const { return _pool; };

	int GetPoolTotal() const { return (int)_pool.size(); };
	int CountAlive() const {
		int n = 0;
		for (const auto& object : _pool) if (object->IsAlive()) n++;
		return n;
	}

	virtual ~PoolManager() = default;

protected:

	T* spawnInPool()
	{
		for (auto& object : _pool)
		{
			if (!object->IsAlive())
			{
				return object.get();
			}
		}

		_pool.push_back(std::make_unique<T>());
		return _pool.back().get();
	}

	std::vector<std::unique_ptr<T>> _pool;
};