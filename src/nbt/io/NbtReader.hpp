#ifndef NBTREADER_HPP
#define NBTREADER_HPP
#include <expected>
#include <memory>
#include <stdexcept>

#include "nbt/Tag.hpp"
#include "nbt/tags/ByteArrayTag.hpp"
#include "nbt/tags/ByteTag.hpp"
#include "nbt/tags/CompoundTag.hpp"
#include "nbt/tags/DoubleTag.hpp"
#include "nbt/tags/FloatTag.hpp"
#include "nbt/tags/Int64Tag.hpp"
#include "nbt/tags/IntArrayTag.hpp"
#include "nbt/tags/IntTag.hpp"
#include "nbt/tags/ListTag.hpp"
#include "nbt/tags/LongArrayTag.hpp"
#include "nbt/tags/ShortTag.hpp"
#include "nbt/tags/StringTag.hpp"

namespace Nbt {

    template <NbtFormat F>
    std::expected<std::unique_ptr<Tag>, std::runtime_error>
    readPayload(cubix::BinaryStream& stream, const TagType type) {
        std::unique_ptr<Nbt::Tag> pTag{nullptr};

        switch (type) {

        case TagType::Byte: {
            pTag = std::make_unique<Nbt::ByteTag<F>>();
            break;
        }
        case TagType::Short: {
            pTag = std::make_unique<Nbt::ShortTag<F>>();
            break;
        }
        case TagType::Int: {
            pTag = std::make_unique<Nbt::IntTag<F>>();
            break;
        }
        case TagType::Int64: {
            pTag = std::make_unique<Nbt::Int64Tag<F>>();
            break;
        }
        case TagType::Float: {
            pTag = std::make_unique<Nbt::FloatTag<F>>();
            break;
        }
        case TagType::Double: {
            pTag = std::make_unique<Nbt::DoubleTag<F>>();
            break;
        }
        case TagType::String: {
            pTag = std::make_unique<Nbt::StringTag<F>>();
            break;
        }
        case TagType::List: {
            pTag = std::make_unique<Nbt::ListTag<F>>();
            break;
        }
        case TagType::Compound: {
            pTag = std::make_unique<Nbt::CompoundTag<F>>();
            break;
        }

        case TagType::ByteArray: {
            pTag = std::make_unique<Nbt::ByteArrayTag<F>>();
            break;
        }
        case TagType::IntArray: {
            pTag = std::make_unique<Nbt::IntArrayTag<F>>();
            break;
        }
        case TagType::LongArray: {
            pTag = std::make_unique<Nbt::LongArrayTag<F>>();
            break;
        }

        default:
            return std::unexpected(std::runtime_error("Unsupported tag"));
        }

        if (pTag != nullptr) {
            if (auto result = pTag->read(stream); !result) {
                return std::unexpected(result.error());
            }

            return pTag;
        };

        return std::unexpected{std::runtime_error{"No NBT tag was constructed."}};
    }

    template <NbtFormat F>
    std::expected<std::unique_ptr<CompoundTag<F>>, std::runtime_error>
    readRoot(cubix::BinaryStream& stream) {
        auto id = stream.tryRead<int8_t>();
        if (!id) {
            return std::unexpected(id.error());
        }

        if (static_cast<TagType>(*id) != TagType::Compound) {
            return std::unexpected(std::runtime_error("Root must be Compound"));
        }

        auto length = stream.tryRead<uint16_t, Nbt::io<F>::Endian>();
        if (!length) {
            return std::unexpected(length.error());
        }

        auto name = stream.tryReadString(*length);
        if (!name) {
            return std::unexpected(name.error());
        }

        auto compound = std::make_unique<CompoundTag<F>>();

        const auto result = compound->read(stream);
        if (!result) {
            return std::unexpected(result.error());
        }

        return compound;
    }

    template <NbtFormat F>
    std::expected<std::pair<std::string, std::unique_ptr<Tag>>, std::runtime_error>
    readNamed(cubix::BinaryStream& stream) {

        // Read type
        auto id = stream.tryRead<int8_t>();
        if (!id) {
            return std::unexpected(id.error());
        }

        const auto type = static_cast<TagType>(*id);

        // End tag -> special case (no name, no payload)
        if (type == TagType::End) {
            return std::pair{"", nullptr};
        }

        // Read name length
        auto length = Nbt::io<F>::template readInt<uint16_t>(stream);
        if (!length) {
            return std::unexpected(length.error());
        }

        // Read name
        auto name = stream.tryReadString(*length);
        if (!name) {
            return std::unexpected(name.error());
        }

        // Read payload
        auto payload = readPayload<F>(stream, type);
        if (!payload) {
            return std::unexpected(payload.error());
        }

        return std::pair{std::move(*name), std::move(*payload)};
    }
} // namespace Nbt

#endif // NBTREADER_HPP
