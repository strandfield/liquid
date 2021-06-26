
# `liquid` 

[![Build Status](https://api.travis-ci.com/strandfield/liquid.svg?branch=master)](https://travis-ci.com/github/strandfield/liquid)
[![codecov](https://codecov.io/gh/strandfield/liquid/branch/master/graph/badge.svg?token=8jDVnpsssJ)](https://codecov.io/gh/strandfield/liquid)

This project is a C++ port of [Shopify's Liquid template engine](https://github.com/Shopify/liquid) (originally written in Ruby).

```cpp
std::string str = "Hello {{ name }}!";

liquid::Template tmplt = liquid::parse(str);

liquid::Map data;
data["name"] = "Alice";
std::string result = tmplt.render(data)
  
assert(result == "Hello Alice!");
```

## Features

Supported tags include:
- `if`, `elsif` and `else`
- `for`
- `break` and `continue`
- `assign`

The default template engine does not provide any filters, but custom filters can easily be implemented by subclassing the `Renderer` class (see `tests.cpp`).



