#pragma once
#include <stdio.h>

#include <nap/resource.h>
#include <nap/logger.h>


extern "C" {
    #include <lua.h>
    #include <lualib.h>
    #include <lauxlib.h>
}

#include <LuaBridge.h>

namespace nap
{

    class NAPAPI LuaScript : public Resource
    {
        RTTI_ENABLE(Resource)
        
    public:
        LuaScript() { };
        
        std::string mPath; ///< Property: 'Path' Path to the Lua script.
        
        bool init(utility::ErrorState& errorState) override;
        
        bool mValid = false; /// < Indicates whether the currently loaded script is valid or has a syntax error.
        
				
		/**
		 * Convenience function. Returns a variable value, if it didn't succeed it logs an error and returns a default constructed object.
		 */
		template <typename T>
		T get(const std::string& identifier);

		
		/**
		 * Convenience function. Calls a function and returns its return value, if it didn't succeed it logs an error and returns a default constructed object.
		 */
        template <typename T, typename... Args>
        T call(const std::string& identifier, Args... args);

		
		/**
		 * Gets a Lua variable. Returns whether it succeeded.
		 */
		template <typename T>
		bool get(const std::string& identifier, utility::ErrorState& errorState, T& value);

		
		/**
		 * Calls a Lua function. Returns whether it succeeded.
		 */
        template <typename ReturnType, typename... Args>
        bool call(const std::string& identifier, utility::ErrorState& errorState, ReturnType& returnValue, Args&... args);
		
		
		/**
		 * Return the Lua namespace to which C++ types and functions can be added.
		 */
		luabridge::Namespace getNamespace()
		{
			return luabridge::getGlobalNamespace(L);
		}
		
    private:
		void bindBasicTypes();
		
        lua_State* L = nullptr;

    };


	template <typename T>
	T LuaScript::get(const std::string& identifier)
	{
		utility::ErrorState e;
		T x;
		if(!get(identifier, e, x))
			Logger::info(e.toString());
		return x;
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


	template <typename T>
	bool LuaScript::get(const std::string& identifier, utility::ErrorState& errorState, T& value)
	{
		luabridge::LuaRef var = luabridge::getGlobal(L, identifier.c_str());
		if(var.isNil())
		{
			errorState.fail("Error getting Lua variable \"%s\": %s", identifier.c_str(), lua_tostring(L, -1));
			return false;
		}
		
		try {
			value = var.template cast<T>().value();
		}
		catch (std::exception const& e)
		{
			errorState.fail("Error getting Lua variable \"%s\": %s", identifier.c_str(), e.what());
			return false;
		}
		
		return true;
		
	}


    template <typename ReturnType, typename ...Args>
    bool LuaScript::call(const std::string& identifier, utility::ErrorState& errorState, ReturnType& returnValue, Args&... args)
    {
        luabridge::LuaRef func = luabridge::getGlobal(L, identifier.c_str());
        
        if (!func.isFunction())
		{
			errorState.fail("Error calling Lua function \"%s\": %s", identifier.c_str(), lua_tostring(L, -1));
			return false;
		}
		
		try {
			auto result = func(args...);
			if(result.size() == 0)
			{
				errorState.fail("Error calling Lua function \"%s\": Function didn't return", identifier.c_str());
				return false;
			}
			returnValue = result[0].template cast<ReturnType>().value();
		}
		catch (std::exception const& e)
		{
			errorState.fail("Error calling Lua function \"%s\": %s", identifier.c_str(), e.what());
			return false;
		}
		
		return true;
	
    }

}

