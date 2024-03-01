#pragma once

#include "BaseModule.h"
#include "BountyModule.h"

class MainModule : public BaseModule
{
public:
	MainModule()
	{

	}
	~MainModule()
	{

	}

	void createChildModules() override
	{
		addModule(std::shared_ptr<BountyModule>(new BountyModule()), this);
	};
};