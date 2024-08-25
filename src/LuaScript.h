/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
// Written by Casimir Geelhoed in 2024.

#pragma once

#include <nap/resource.h>
#include <nap/logger.h>

extern "C" {
	#include <lua.h>
	#include <lualib.h>
	#include <lauxlib.h>
}

#include "LuaBridge/LuaBridge.h"

namespace nap
{

	/**
	 * A Resource that manages a single Lua script file.
	 */
	class NAPAPI LuaScript : public Resource
	{
		RTTI_ENABLE(Resource)
		
	public:
		LuaScript() { };
		
		std::string mPath; ///< Property: 'Path' Path to the Lua script.
		
		bool init(utility::ErrorState& errorState) override;
		
		bool mValid = false; ///< Indicates whether the currently loaded script is valid or has a syntax error.
		
		/**
		 * Returns a variable value, if it didn't succeed it logs an error and returns a default constructed object.
		 * @param identifier the name of the variable in Lua
		 * @return the value of the variable
		 */
		template <typename T>
		T getVariable(const std::string& identifier);
		
		/**
		 * Gets a Lua variable. Returns whether it succeeded.
		 * @param identifier the name of the variable in Lua
		 * @param errorState contains the error if getting the variable value fails
		 * @param outValue the value of the variable
		 * @return whether it succeeded to get the variable value from Lua
		 */
		template <typename T>
		bool getVariable(const std::string& identifier, utility::ErrorState& errorState, T& outValue);

		
		/**
		 * Calls a function and returns its return value, if it didn't succeed it logs an error and returns a default constructed object
		 * @param identifier the name of the function in Lua
		 * @param args arguments to the function in Lua
		 * @return the return value of the variable
		 */
		template <typename T, typename... Args>
		T call(const std::string& identifier, Args... args);
		
		/**
		 * Calls a Lua function with a single return value. Returns whether it succeeded.
		 * @param identifier the name of the function in Lua
		 * @param errorState contains the error if calling the function fails
		 * @param outReturnValue the return value of the function
		 * @param args arguments to the function in Lua
		 * @return whether it succeede to call the function in Lua
		 */
		template <typename ReturnType, typename... Args>
		bool call(const std::string& identifier, utility::ErrorState& errorState, ReturnType& outReturnValue, Args&... args);

		
		/**
		 * Calls a function without return value, if it didn't succeed it logs an error.
		 * @param args arguments to the function in Lua
		 */
		template <typename... Args>
		void callVoid(const std::string& identifier, Args... args);
				
		/**
		 * Calls a Lua function without return value. Returns whether it succeeded.
		 * @param identifier the name of the function in Lua
		 * @param errorState contains the error if calling the function fails
		 * @param returnValue the return value of the function
		 * @param args arguments to the function in Lua
		 * @return whether it succeede to call the function in Lua
		 */
		template <typename... Args>
		bool callVoid(const std::string& identifier, utility::ErrorState& errorState, Args&... args);
		
		/**
		 * Return the Lua namespace to which custom C++ types and functions can be added.
		 * @return the Lua namespace
		 */
		luabridge::Namespace getNamespace() { return luabridge::getGlobalNamespace(L); }
		
	private:
		void bindBasicTypes();
		
		lua_State* L = nullptr;
		
	};


	template <typename T>
	T LuaScript::getVariable(const std::string& identifier)
	{
		utility::ErrorState e;
		T x;
		if(!get(identifier, e, x))
			Logger::info(e.toString());
		return x;
	}


	template <typename T>
	bool LuaScript::getVariable(const std::string& identifier, utility::ErrorState& errorState, T& outValue)
	{
		luabridge::LuaRef var = luabridge::getGlobal(L, identifier.c_str());
		if(var.isNil())
		{
			errorState.fail("Error getting Lua variable \"%s\": %s", identifier.c_str(), lua_tostring(L, -1));
			return false;
		}
		
		try 
		{
			outValue = var.template cast<T>().value();
		}
		catch (std::exception const& e)
		{
			errorState.fail("Error getting Lua variable \"%s\": %s", identifier.c_str(), e.what());
			return false;
		}
		
		return true;
	}


	template <typename T, typename... Args>
	T LuaScript::call(const std::string& identifier, Args... args)
	{
		utility::ErrorState e;
		T x;
		if(!call(identifier, e, x, args...))
			Logger::info(e.toString());
		return x;
	}


	template <typename ReturnType, typename ...Args>
	bool LuaScript::call(const std::string& identifier, utility::ErrorState& errorState, ReturnType& outReturnValue, Args&... args)
	{
		luabridge::LuaRef func = luabridge::getGlobal(L, identifier.c_str());
		
		if (!func.isFunction())
		{
			errorState.fail("Error calling Lua function \"%s\": %s", identifier.c_str(), lua_tostring(L, -1));
			return false;
		}
		
		try
		{
			auto result = func(args...);
			if(result.size() == 0)
			{
				errorState.fail("Error calling Lua function \"%s\": Function didn't return", identifier.c_str());
				return false;
			}
			outReturnValue = result[0].template cast<ReturnType>().value();
		}
		catch (std::exception const& e)
		{
			errorState.fail("Error calling Lua function \"%s\": %s", identifier.c_str(), e.what());
			return false;
		}
		
		return true;
	}


	template <typename... Args>
	void LuaScript::callVoid(const std::string& identifier, Args... args)
	{
		utility::ErrorState e;
		if(!callVoid(identifier, e, args...))
			Logger::info(e.toString());
	}


	template <typename ...Args>
	bool LuaScript::callVoid(const std::string& identifier, utility::ErrorState& errorState, Args&... args)
	{
		luabridge::LuaRef func = luabridge::getGlobal(L, identifier.c_str());
		
		if (!func.isFunction())
		{
			errorState.fail("Error calling Lua function \"%s\": %s", identifier.c_str(), lua_tostring(L, -1));
			return false;
		}
		
		try
		{
			func(args...);
		}
		catch (std::exception const& e)
		{
			errorState.fail("Error calling Lua function \"%s\": %s", identifier.c_str(), e.what());
			return false;
		}
		
		return true;
	}

}

