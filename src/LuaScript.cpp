/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
// Written by Casimir Geelhoed in 2024.

#include "LuaScript.h"

#include <utility/fileutils.h>

#include <glm/glm.hpp>

RTTI_BEGIN_CLASS(nap::LuaScript)
	RTTI_PROPERTY_FILELINK("Path", &nap::LuaScript::mPath, nap::rtti::EPropertyMetaData::Required, nap::rtti::EPropertyFileType::Any)
RTTI_END_CLASS

namespace nap
{

	bool LuaScript::init(utility::ErrorState& errorState)
	{
		// Read file to string.
		if (!utility::readFileToString(mPath, mScriptAsString, errorState))
			return false;
		
		// Create Lua state.
		L = luaL_newstate();
		
		// Add libraries.
		luaL_openlibs(L);
		
		// Enable exceptions.
		luabridge::LuaException::enableExceptions(L);
				
		return true;
	}
	
	
	bool LuaScript::load(utility::ErrorState& errorState)
	{
		// Load the script.
		int r = luaL_dostring(L, mScriptAsString.c_str());
		if (r != LUA_OK)
		{
			mValid = false;
			errorState.fail("Lua script invalid: %s", lua_tostring(L, -1));
			return false;
		}
		
		mValid = true;
		return true;
	}

}
