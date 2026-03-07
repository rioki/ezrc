# ezrc

## About

`ezrc` is a command line utility that packs files into C++ source code.

This allows static resources such as shaders, images or text files to be embedded 
directly into a program binary. The result is a robust and portable solution when 
application data is tightly coupled to source code.

Resources are defined in a YAML file and compiled into a C++ header that exposes 
a simple lookup API.

---

## Usage

```
ezrc <resources.yml>
```

The input is a YAML configuration file describing the namespace and files to embed.

---

## Configuration

Example configuration:

```yaml
namespace: test
files:
  - lena.png
  - math.glsl
```

* `namespace`
  The C++ namespace used for the generated API.

* `files`
  A list of files that will be embedded into the generated source.

---

## Generated API

`ezrc` generates a C++ header exposing a simple resource lookup function.

Example:

```cpp
// File generated with ezrc

#pragma once

#include <string_view>

namespace test
{
    std::string_view get_resource(std::string_view file);
}
```

The `file` parameter corresponds to the original filename specified in the 
YAML configuration.

Example usage:

```cpp
#include "resources.h"

auto shader = test::get_string_resource("math.glsl");
```

The returned `std::string_view` references the embedded resource data.

## Building

`ezrc` can be built in several ways.

### Using vcpkg

```
vcpkg install ezrc
```

### Using CMake

```
cmake -B build
cmake --build build
```

### Using Visual Studio

Open the `ezrc.sln` in Visual Studio and build the solution.

---

## License

`ezrc` is distributed under the MIT License.
