#ifndef BYTEARRAYTAG_HPP
#define BYTEARRAYTAG_HPP
#include <ranges>
#include <string>

#include "ArrayTagBase.hpp"
#include "nbt/Tag.hpp"
#include "nbt/io/NbtFormat.hpp"

namespace Nbt {

    template <NbtFormat F>
    class ByteArrayTag final : public ArrayTagBase<int8_t, F> {
    private:
        using Base = ArrayTagBase<int8_t, F>;

    public:
        using Base::Base;

        [[nodiscard]] Nbt::TagType getType() const override {
            return Nbt::TagType::ByteArray;
        }

        void write(cubix::BinaryStream& stream) override {
            this->writeImpl(stream, [](cubix::BinaryStream& s, const int8_t byte) {
                s.writeInt8(byte);
            });
        };

        [[nodiscard]] std::expected<void, std::runtime_error>
        read(cubix::BinaryStream& stream) override {
            return this->readImpl(stream, [](cubix::BinaryStream& s) {
                return s.tryRead<int8_t>();
            });
        }

        [[nodiscard]] std::shared_ptr<Nbt::Tag> copy() const override {
            return std::make_shared<ByteArrayTag>(*this);
        }

        [[nodiscard]] std::string toString(int /*indent*/) const override {
            auto transformed = this->elements() | std::views::transform([](auto v) {
                                   return std::to_string(v) + "b";
                               });

            std::string result = "[B; ";
            bool        first  = true;

            for (const auto& str : transformed) {
                if (!first) {
                    result += ", ";
                }

                first   = false;
                result += str;
            }

            result += "]";
            return result;
        }
    };
} // namespace Nbt

#endif // !BYTEARRAYTAG_HPP