#pragma once

class PoolObject
{
public:
	virtual ~PoolObject() = default;

	virtual void Update(float dt) = 0;
	virtual void Draw() = 0;

	virtual void Deactivate() { _alive = false; };

	bool IsAlive() const { return _alive; }

protected:
	bool _alive = true;
};