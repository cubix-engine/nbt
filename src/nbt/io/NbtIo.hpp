#ifndef NBTIO_HPP
#define NBTIO_HPP
#include <bit>

#include "NbtFormat.hpp"
#include <BinaryStream/BinaryStream.hpp>

namespace Nbt {
    inline std::runtime_error toRuntimeError(const std::runtime_error& e) {
        return e;
    }

    inline std::runtime_error
    toRuntimeError(const cubix::BinaryStreamException::ReadErrorInfo& e) {
        return std::runtime_error{e.message};
    }

    template <NbtFormat F>
    struct io {
        static constexpr std::endian Endian =
            (F == NbtFormat::BigEndian) ? std::endian::big : std::endian::little;

        template <typename T>
        static std::expected<T, std::runtime_error> readInt(cubix::BinaryStream& stream) {
            if constexpr (F == NbtFormat::NetworkLittleEndian) {
                if constexpr (std::is_same_v<T, int32_t>) {
                    auto value = stream.tryReadVarInt32();
                    if (!value) {
                        return std::unexpected(std::runtime_error{value.error().message});
                    }

                    return *value;
                }
            }

            auto value = stream.tryRead<T, Endian>();
            if (!value) {
                return std::unexpected(std::runtime_error{value.error().message});
            }

            return *value;
        }

        static std::expected<std::string, std::runtime_error>
        readString(cubix::BinaryStream& stream) {
            uint16_t length;
            if constexpr (F == NbtFormat::NetworkLittleEndian) {
                auto value = stream.tryReadVarInt32();
                if (!value) {
                    return std::unexpected(std::runtime_error{value.error().message});
                }

                length = static_cast<uint16_t>(*value);
            }
            else {
                auto value = stream.tryRead<uint16_t, Endian>();
                if (!value) {
                    return std::unexpected(std::runtime_error{value.error().message});
                }

                length = *value;
            }

            auto value = stream.tryReadString(length);
            if (!value) {
                return std::unexpected(std::runtime_error{value.error().message});
            }

            return *value;
        }

        template <typename T>
        static void writeInt(cubix::BinaryStream& stream, T value) {
            if constexpr (F == NbtFormat::NetworkLittleEndian) {
                if constexpr (std::is_same_v<T, int32_t>) {
                    stream.writeVarInt32(value);
                    return;
                }
                else if constexpr (std::is_same_v<T, int64_t>) {
                    stream.writeVarInt64(value);
                    return;
                }
            }

            stream.write<T, Endian>(value);
        }

        static void writeString(cubix::BinaryStream& stream, const std::string& value) {
            const auto length = static_cast<uint16_t>(value.size());
            if constexpr (F == NbtFormat::NetworkLittleEndian) {
                stream.writeVarUint32(length);
            }
            else {
                stream.writeInt16(static_cast<int16_t>(value.size()));
            }

            stream.writeString(value, length);
        }

        template <typename T, typename Fn>
        static std::expected<std::vector<T>, std::runtime_error>
        readList(cubix::BinaryStream& stream, Fn&& fn) {
            auto length = readInt<int32_t>(stream);
            if (!length) {
                return std::unexpected(length.error());
            }

            if (*length < 0) {
                return std::unexpected(std::runtime_error("Negative list length"));
            }

            std::vector<T> result{};
            result.reserve(static_cast<size_t>(*length));

            for (int32_t i = 0; i < *length; ++i) {
                auto value = fn(stream);

                if (!value) {
                    return std::unexpected(toRuntimeError(value.error()));
                }

                result.push_back(*value);
            }

            return result;
        }

        template <typename T, typename Fn>
        static std::expected<void, std::runtime_error>
        writeList(cubix::BinaryStream& stream, const std::vector<T>& list, Fn&& fn) {
            const auto size = static_cast<int32_t>(list.size());
            writeInt<int32_t>(stream, size);

            for (const auto& item : list) {
                fn(stream, item);
            }

            return {};
        }
    };
} // namespace Nbt

#endif // NBTIO_HPP
