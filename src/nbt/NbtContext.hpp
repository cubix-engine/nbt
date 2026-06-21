#ifndef NBTCONTEXT_HPP
#define NBTCONTEXT_HPP

#include "io/NbtFormat.hpp"
#include "io/NbtReader.hpp"
#include "io/NbtWriter.hpp"

#include "tags/ByteArrayTag.hpp"
#include "tags/ByteTag.hpp"
#include "tags/CompoundTag.hpp"
#include "tags/DoubleTag.hpp"
#include "tags/FloatTag.hpp"
#include "tags/Int64Tag.hpp"
#include "tags/IntArrayTag.hpp"
#include "tags/IntTag.hpp"
#include "tags/ListTag.hpp"
#include "tags/LongArrayTag.hpp"
#include "tags/ShortTag.hpp"
#include "tags/StringTag.hpp"

namespace Nbt {
    template <Nbt::NbtFormat F>
    struct NbtContext {
        using ByteTag      = Nbt::ByteTag<F>;
        using ShortTag     = Nbt::ShortTag<F>;
        using IntTag       = Nbt::IntTag<F>;
        using Int64Tag     = Nbt::Int64Tag<F>;
        using FloatTag     = Nbt::FloatTag<F>;
        using DoubleTag    = Nbt::DoubleTag<F>;
        using StringTag    = Nbt::StringTag<F>;
        using ListTag      = Nbt::ListTag<F>;
        using CompoundTag  = Nbt::CompoundTag<F>;
        using ByteArrayTag = Nbt::ByteArrayTag<F>;
        using IntArrayTag  = Nbt::IntArrayTag<F>;
        using LongArrayTag = Nbt::LongArrayTag<F>;

        static auto readRoot(cubix::BinaryStream& s) {
            return Nbt::readRoot<F>(s);
        }

        static auto writeRoot(cubix::BinaryStream& s, std::unique_ptr<Nbt::Tag> tag) {
            return Nbt::writeRoot<F>(s, std::move(tag));
        }
    };
} // namespace Nbt

#endif // NBTCONTEXT_HPP
