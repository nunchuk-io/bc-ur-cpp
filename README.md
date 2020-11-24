# UR-CPP

Implementation of [BCR-0004](https://github.com/CoboVault/Research/blob/master/papers/bcr-0004-bc32.md) and [BCR-0005](https://github.com/CoboVault/Research/blob/master/papers/bcr-0005-ur.md)

## Installation

Add `ur-cpp` as a submodule in a larger CMake project.

```bash
$ cd your_project/
$ git submodule add https://github.com/nunchuk-io/ur-cpp
$ git submodule update --init --recursive
```

Add the following to your `CMakeLists.txt`.

```cmake
add_subdirectory(ur-cpp)
target_link_libraries("${PROJECT_NAME}" PUBLIC ur)
```

## Usage

Include `<bc32.h>` to encode or decode BC32 data encoding format 

```c++
#include <bc32.h>

using namespace nunchuk::bcr;
 
std::string bc32Data = EncodeBc32Data(HexToBytes("..."));
std::vector<uint8_t> raw = DecodeBc32Data(bc32Data);
```

Include `<ur.h>` to encode or decode Uniform Resources

```c++
#include <ur.h>

using namespace nunchuk::bcr;
 
std::vector<std::string> workloads = EncodeUniformResource(HexToBytes("...");
std::vector<uint8_t> raw = DecodeUniformResource(workloads);
```

##  License

`ur-cpp` is released under the terms of the MIT license. See [COPYING](COPYING) for more information or see https://opensource.org/licenses/MIT.
