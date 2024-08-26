# naplua

This is a [NAP Framework](https://github.com/napframework/nap) module that embeds a [Lua](https://www.lua.org/) script as a NAP resource using [LuaBridge3](https://github.com/kunitoki/LuaBridge3).
Each LuaScript resource manages a distinct LuaState, to which C++ functions and types can be exposed and from which variables can be read and functions can be called. It works great with real-time editing.

_MacOS (Intel) & MacOS (Silicon) are supported._	

# Usage examples
## Lua to C++
		

#### Getting a variable value from Lua:
```
x = 123
```
Getting the variable (logs an error if it fails):
```
int x = mLuaScript->getVariable<int>("x");
```
Getting the variable while checking if it exists:
```
utility::ErrorState e;
int x;
if(!mLuaScript->getVariable("x", e, x))
  Logger::info(e.toString());
```

#### Calling a Lua function:
```
function add(a, b)
  return a + b
end
```
Calling the Lua function (logs an error if it fails):
```
int output = mLuaScript->call<int>("add", 3, 4);
```
Calling the Lua function and getting its return value back, while checking if the function call succeeds:
```
int output = 0;
utility::ErrorState e;
if(!mLuaScript->call("add", e, output, 3, 4))
  Logger::info(e.toString());
```

## C++ to Lua
		
#### Exposing a C++ function to Lua:

```
mLuaScript->getNamespace().addFunction("CppFunc", &CppFunc);
```

#### Exposing a C++ member function to Lua (by wrapping the function in a lambda):

```
  float MyClass::CppMemberFunc(float a, float b)
  {
    return a + b;
  }
```

```
mLuaScript->getNamespace().addFunction("CppMemberFunc", [&](float a, float b) { return CppMemberFunc(a, b); });
```



#### Exposing custom C++ types to Lua: 

The below example binds the glm::vec3 to Lua, using static functions to define getters / setters and arithmetic operators. For more information, see the [LuaBridge3 manual](https://github.com/kunitoki/LuaBridge3/blob/master/Manual.md).

```
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


void LuaBindingResource::bindBasicTypes()
{
	// bind glm::vec3
	mLuaScript->getNamespace().beginClass<glm::vec3>("vec3")
	.addConstructor<void(*)(float, float, float)>()
	.addProperty ("x", &VecHelper::get<0>, &VecHelper::set<0>)
	.addProperty ("y", &VecHelper::get<1>, &VecHelper::set<1>)
	.addProperty ("z", &VecHelper::get<2>, &VecHelper::set<2>)
	.addFunction("__add", &VecHelper::add)
	.addFunction("__sub", &VecHelper::sub)
	.addFunction("__mul", &VecHelper::mul)
	.addFunction("__div", &VecHelper::div)
	.endClass();

	// reload Lua script
	mLuaScript->load();
}
```

Make sure to call these bindings from the init() function of an Object that points to the LuaScript resource, so that the bindings are re-added directly after the LuaScript reloads at runtime.
