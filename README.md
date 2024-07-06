# naplua

This is a [NAP Framework](https://github.com/napframework/nap) module that embeds a [Lua](https://www.lua.org/) script as a NAP resource.
Each LuaScript resource manages a distinct LuaState, to which C++ functions and types can be exposed and from which variables can be read and functions can be called. It works great with real-time editing.

_Note: for now, this is only tested and maintained on MacOS (Intel). Windows/Linux could come later._	

# Usage examples
## Lua to C++
		
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
		
Exposing a C++ function to Lua:

```
mLuaScript->getNamespace().addFunction("CppFunc", &CppFunc);
```

Exposing a C++ member function to Lua (by wrapping the function in a lambda):

```
  float MyClass::CppMemberFunc(float a, float b)
  {
    return a + b;
  }
```

```
mLuaScript->getNamespace().addFunction("CppMemberFunc", [&](float a, float b) { return CppMemberFunc(a, b); });
```



Exposing custom C++ types to Lua: 
See the function LuaScript::bindBasicTypes for examples.

