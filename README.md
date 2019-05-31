
# `liquid` 

This project is a C++ port of [Shopify's Liquid template engine](https://github.com/Shopify/liquid) (originally written in Ruby).

The project's only dependencies are the C++ standard library and [json-toolkit](https://github.com/bandicode/json-toolkit).

```cpp
std::string str = "Hello {{ name }}!";

liquid::Template tmplt = liquid::parse(str);

json::Object data = {};
data["name"] = "Alice";
std::string result = tmplt.render(data)
  
assert(result, "Hello Alice!");
```

## Features

Supported tags include:
- `if`, `elsif` and `else`
- `for`
- `break` and `continue`
- `assign`

The default template engine does not provide any filters, but custom filters can easily be implemented by subclassing the `Renderer` class (see `tests.cpp`).



