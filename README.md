# naplua

This is a [NAP Framework](https://github.com/napframework/nap) module that embeds a [Lua](https://www.lua.org/) script as a NAP resource.
Each LuaScript resource manages a distinct LuaState, to which C++ functions and types can be exposed and from which variables can be read and functions can be called. It works great with real-time editing.

_Note: this is only tested and maintained on MacOS (Intel). Windows/Linux support will come later._	

# Usage:
## Lua > C++
		
Example Lua Script:

```
x = 123
		 
function add(a, b)
  return a + b
end
```
				
Getting a variable value from Lua (printing an error if it fails):
```
int x = mLuaScript->get<int>("x");
```

Getting a value from Lua, with ErrorState:
```
utility::ErrorState e;
int x;
if(!mLuaScript->get("x", e, x))
  Logger::info(e.toString());
```

Calling a Lua function and getting its return value back (printing an error if it fails):
```
int output = mLuaScript->call<int>("add", 3, 4);
```
		
Calling a Lua function and getting its return value back, with ErrorState:
```
float inputA = 2.0f;
float inputB = 3.0f;
float output = 0.f;
utility::ErrorState e;
if(!mLuaScript->call("add", e, output, inputA, inputB))
  Logger::info(e.toString());
```
## C++ to Lua
		
		
Example C++ function:

```
  float CppAdd(float a, float b)
  {
    return a + b;
  }
```
Exposing a C++ function to Lua:
```
mLuaScript->getNamespace().addFunction("CppAdd", &CppAdd);
```

Exposing custom C++ types to Lua: see the function LuaScript::bindBasicTypes for examples.

