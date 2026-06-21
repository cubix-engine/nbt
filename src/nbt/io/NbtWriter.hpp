#ifndef NBTWRITER_HPP
#define NBTWRITER_HPP
#include <expected>
#include <stdexcept>

#include "NbtFormat.hpp"
#include "NbtIo.hpp"
#include "nbt/Tag.hpp"

namespace Nbt {
    inline void writePayload(cubix::BinaryStream& stream, const std::unique_ptr<Nbt::Tag>& tag) {
        tag->write(stream);
    }

    template <NbtFormat F>
    void writeNamed(
        cubix::BinaryStream& stream, const std::string& name, const std::unique_ptr<Tag>& tag
    ) {
        // Write type
        stream.writeInt8(static_cast<int8_t>(tag->getType()));

        // End tag = no name, no payload
        if (tag->getType() == TagType::End) {
            return;
        }

        // Write name length
        auto length = static_cast<uint16_t>(name.size());
        Nbt::io<F>::template writeInt<uint16_t>(stream, length);

        // Write name
        stream.writeBytes(reinterpret_cast<const uint8_t*>(name.data()), length);

        // Write payload
        return writePayload(stream, tag);
    }

    template <NbtFormat F>
    std::expected<void, std::runtime_error>
    writeRoot(cubix::BinaryStream& stream, const std::unique_ptr<Tag>& root) {
        if (root->getType() != TagType::Compound) {
            return std::unexpected(std::runtime_error("Root must be Nbt::CompoundTag"));
        }

        writeNamed<F>(stream, "", root);
        return {};
    }
} // namespace Nbt

#endif // NBTWRITER_HPP
