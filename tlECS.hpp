#pragma once

#include <iostream>
#include <unordered_map>
#include <memory>
#include <vector>
#include <typeinfo>
#include <typeindex>

namespace tl
{
	class Component
	{
	public:
		virtual ~Component() = default;
	};

	class Entity
	{
	public:
		Entity() = default;
		Entity(Entity&&) = default;
		Entity(const Entity&) = delete;
		~Entity() = default;
		Entity& operator=(Entity&&) = default;
		Entity& operator=(const Entity&) = delete;

		template<typename T>
		T& access()
		{
			return dynamic_cast<T&>(*components.at(std::type_index{ typeid(T) }).get());
		}

		template<typename T>
		const bool contains() const
		{
			return components.find(std::type_index{ typeid(T) }) != components.end();
		}

		template<typename... Args>
		void attach()
		{
			(singleAttach<Args>(), ...);
		}

		template<typename... Args>
		void detach()
		{
			(singleDetach<Args>(), ...);
		}
	private:
		std::unordered_map<std::type_index, std::unique_ptr<Component>> components;

		template<typename T>
		void singleAttach()
		{
			if (components.find(std::type_index{ typeid(T) }) == components.end())
			{
				components[std::type_index{ typeid(T) }] = std::make_unique<T>();
			}
		}

		template<typename T>
		void singleDetach()
		{
			if (contains<T>())
			{
				components.erase(std::type_index{ typeid(T) });
			}
		}
	};

	class System
	{
	public:
		template<typename... Args>
		static const std::vector<size_t> query(const std::vector<Entity>& entities)
		{
			std::vector<size_t> indices;

			for (size_t i{}; i < entities.size(); ++i)
			{
				if ((entities[i].contains<Args>() && ...))
				{
					indices.emplace_back(i);
				}
			}

			return indices;
		}
	};
}
