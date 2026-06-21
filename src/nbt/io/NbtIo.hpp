#ifndef NBTIO_HPP
#define NBTIO_HPP
#include <bit>
#include <cstring>

#include "NbtFormat.hpp"
#include <BinaryStream/BinaryStream.hpp>

namespace Nbt {
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
                        return std::unexpected(value.error());
                    }

                    return *value;
                }
            }

            auto value = stream.tryRead<T, Endian>();
            if (!value) {
                return std::unexpected(value.error());
            }

            return *value;
        }

        static std::expected<std::string, std::runtime_error>
        readString(cubix::BinaryStream& stream) {
            uint16_t length;
            if constexpr (F == NbtFormat::NetworkLittleEndian) {
                auto value = stream.tryReadVarInt32();
                if (!value) {
                    return std::unexpected(value.error());
                }

                length = static_cast<uint16_t>(*value);
            }
            else {
                auto value = stream.tryRead<uint16_t, Endian>();
                if (!value) {
                    return std::unexpected(value.error());
                }

                length = *value;
            }

            auto value = stream.tryReadString(length);
            if (!value) {
                return std::unexpected(value.error());
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
                    return std::unexpected(value.error());
                }

                result.emplace_back(std::move(*value));
            }

            return result;
        }

        template <typename T, typename Fn>
        static void
        writeList(cubix::BinaryStream& stream, const std::vector<T>& list, Fn&& fn) {
            const auto size = static_cast<int32_t>(list.size());
            writeInt<int32_t>(stream, size);

            for (const auto& item : list) {
                fn(stream, item);
            }
        }

        template <typename T>
        static std::expected<std::vector<T>, std::runtime_error>
        readArray(cubix::BinaryStream& stream) {
            static_assert(
                std::is_trivially_copyable_v<T>, "readArray requires a trivially copyable type"
            );

            auto length = readInt<int32_t>(stream);
            if (!length) {
                return std::unexpected(length.error());
            }

            if (*length < 0) {
                return std::unexpected(std::runtime_error("Negative array length"));
            }

            const auto count = static_cast<size_t>(*length);
            if (count > stream.bytesLeft() / sizeof(T)) {
                return std::unexpected(std::runtime_error("Array length too large"));
            }

            auto bytes = stream.tryReadBytes(count * sizeof(T));
            if (!bytes) {
                return std::unexpected(std::runtime_error(bytes.error().what()));
            }

            std::vector<T> result(count);
            std::memcpy(result.data(), bytes->data(), bytes->size());

            if constexpr (Endian != std::endian::native) {
                for (auto& value : result) {
                    value = cubix::BinaryStream::adjustEndian<T, Endian>(value);
                }
            }

            return result;
        }

        template <typename T>
        static void writeArray(cubix::BinaryStream& stream, const std::vector<T>& values) {
            static_assert(
                std::is_trivially_copyable_v<T>, "writeArray requires a trivially copyable type"
            );

            writeInt<int32_t>(stream, static_cast<int32_t>(values.size()));

            if (values.empty()) {
                return;
            }

            if constexpr (Endian == std::endian::native) {
                stream.writeBytes(
                    reinterpret_cast<const uint8_t*>(values.data()), values.size() * sizeof(T)
                );
            }
            else {
                std::vector<T> swapped(values.size());
                for (size_t i = 0; i < values.size(); ++i) {
                    swapped[i] = cubix::BinaryStream::adjustEndian<T, Endian>(values[i]);
                }

                stream.writeBytes(
                    reinterpret_cast<const uint8_t*>(swapped.data()), swapped.size() * sizeof(T)
                );
            }
        }
    };
} // namespace Nbt

#endif // NBTIO_HPP
