#include <print>

#include <nbt/tags/ByteArrayTag.hpp>
#include <nbt/tags/ByteTag.hpp>
#include <nbt/tags/CompoundTag.hpp>
#include <nbt/tags/DoubleTag.hpp>
#include <nbt/tags/FloatTag.hpp>
#include <nbt/tags/Int64Tag.hpp>
#include <nbt/tags/IntArrayTag.hpp>
#include <nbt/tags/IntTag.hpp>
#include <nbt/tags/ListTag.hpp>
#include <nbt/tags/LongArrayTag.hpp>
#include <nbt/tags/ShortTag.hpp>
#include <nbt/tags/StringTag.hpp>

#include <nbt/NbtContext.hpp>
using NbtLE = Nbt::NbtContext<Nbt::NbtFormat::LittleEndian>;

int main(int /*argc*/, char* /*argv*/[]) {
    cubix::BinaryStream stream{};

    NbtLE::ListTag list{};
    list.add(NbtLE::ByteTag{1});
    list.add(NbtLE::ByteTag{2});
    list.add(NbtLE::ByteTag{3});

    NbtLE::CompoundTag compound{};

    NbtLE::CompoundTag tag{};
    tag.add("int8", NbtLE::ByteTag(1));
    tag.add("int16", NbtLE::ShortTag(1));
    tag.add("int32", NbtLE::IntTag(1));
    tag.add("int64", NbtLE::Int64Tag(1));
    tag.add("float", NbtLE::FloatTag(1.0));
    tag.add("double", NbtLE::DoubleTag(1.0));
    tag.add("string", NbtLE::StringTag("Hello, world!"));
    tag.add("list", list);
    tag.add("compound", compound);
    tag.add("byteArray", NbtLE::ByteArrayTag{0, 1, 2});
    tag.add("intArray", NbtLE::IntArrayTag{3, 4, 5});
    tag.add("longArray", NbtLE::LongArrayTag{6, 7, 8});

    std::ignore      = NbtLE::writeRoot(stream, tag.copy());
    const auto value = NbtLE::readRoot(stream);
    if (!value) {
        return 1;
    }

    const auto& result = *value;
    std::println("{}", result->toString());
}