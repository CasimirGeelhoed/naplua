
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
		// read file to string
		std::string scriptString;
		if (!utility::readFileToString(mPath, scriptString, errorState))
			return false;
		
		// create Lua state
		L = luaL_newstate();
		
		// add libraries
		luaL_openlibs(L);
		
		// enable exceptions
		luabridge::LuaException::enableExceptions(L);
		
		// bind basic types
		bindBasicTypes();
		
		// load script
		int r = luaL_dostring(L, scriptString.c_str());
		if (r == LUA_OK)
		{
			mValid = true;
		}
		else
		{
			mValid = false;
			Logger::info("Lua script invalid: %s", lua_tostring(L, -1));
		}
		
		// return (we also return true if the script is invalid, allowing the user to fix it while the app is running)
		return true;
	}


	struct VecHelper
	{
		template <unsigned index>
		static float get (glm::vec3 const* vec)
		{
			return (*vec)[index];
		}
		
		template <unsigned index>
		static void set (glm::vec3* vec, float value)
		{
			(*vec)[index] = value;
		}
		
		static glm::vec3 add(const glm::vec3& l, const glm::vec3& r) {
			return l + r;
		}
		
		static glm::vec3 sub(const glm::vec3& l, const glm::vec3& r) {
			return l - r;
		}
		
		static glm::vec3 mul(const glm::vec3& v, float scalar) {
			return v * scalar;
		}
		
		static glm::vec3 div(const glm::vec3& v, float scalar) {
			return v / scalar;
		}
		
	};


	void LuaScript::bindBasicTypes()
	{
		// bind glm::vec3
		getNamespace().beginClass<glm::vec3>("vec3")
		.addConstructor<void(*)(float, float, float)>()
		.addProperty ("x", &VecHelper::get<0>, &VecHelper::set<0>)
		.addProperty ("y", &VecHelper::get<1>, &VecHelper::set<1>)
		.addProperty ("z", &VecHelper::get<2>, &VecHelper::set<2>)
		.addFunction("__add", &VecHelper::add)
		.addFunction("__sub", &VecHelper::sub)
		.addFunction("__mul", &VecHelper::mul)
		.addFunction("__div", &VecHelper::div)
		.endClass();
	}

}
