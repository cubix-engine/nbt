<div align="center">

  # NBT
A modern, fast, and type-safe C++ library for reading, writing, and manipulating Named Binary Tag (NBT) data.

  [![C++][CPP_BADGE_URL]][CPP_URL]
</div>

---

## Features
- Fully **typed tag system**
- **Read & write support** for NBT
- Supports multiple formats via `NbtFormat` (e.g. Little Endian)
- Clean **template-based architecture**

## Library Example
```cpp
cubix::BinaryStream stream{};

using NbtLE = Nbt::NbtContext<Nbt::NbtFormat::LittleEndian>;

NbtLE::CompoundTag compound{};
compound.add("int8", NbtLE::ByteTag(1));

// Write
std::ignore = NbtLE::writeRoot(stream, tag.copy());

// Read
const auto result = NbtLE::readRoot(stream);
if (!result) {
    return 1;
}

std::println("{}", result.value()->toString());
```

## Installation (CMake)
```cmake
FetchContent_Declare(
    NBT
    GIT_REPOSITORY https://github.com/Cubix-Engine/NBT.git
    GIT_TAG main
)

FetchContent_MakeAvailable(NBT)

target_link_libraries(your_target PRIVATE NBT::nbt)
```

## License
This project is open-source under the **MIT License**. Feel free to modify and contribute!

## Contributing
```shell
# Fork the repository on GitHub, then:
git clone https://github.com/<your-username>/NBT.git

# Create a new branch
git checkout -b feature-branch

# Commit your changes
git commit -m "Added new feature"

# Push to GitHub
git push origin feature-branch

# Submit a Pull Request
```

<!-- BADGES -->
[CPP_BADGE_URL]: https://img.shields.io/badge/C++-23-%2300599C?style=flat-square&logo=cplusplus&logoColor=%2300599C&labelColor=white
[CPP_URL]: https://cplusplus.com
<!-- BADGES -->
