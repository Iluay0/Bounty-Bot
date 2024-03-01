#pragma once

#include <dpp/dpp.h>

#include <memory>
#include <string>
#include <map>

#include "dpp/channel.h"

namespace util
{
	extern nlohmann::json env;

	inline const dpp::component& getFormComponent(const dpp::form_submit_t& form, std::string customId)
	{
		const auto& formComponents = form.components;
		auto it = std::find_if(formComponents.begin(), formComponents.end(),
			[&](const dpp::component& component)
			{
				return component.components[0].custom_id == customId;
			});

		if (it == formComponents.end())
			return dpp::component();

		return it->components[0];
	}

	inline void replace(std::string& string, std::string from, std::string to)
	{
		string.replace(string.find(from), from.size(), to);
	}
};