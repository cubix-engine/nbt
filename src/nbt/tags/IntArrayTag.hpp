#ifndef INTARRAYTAG_HPP
#define INTARRAYTAG_HPP

#include "ArrayTagBase.hpp"
#include "nbt/Tag.hpp"
#include "nbt/io/NbtFormat.hpp"

namespace Nbt {

    template <NbtFormat F>
    class IntArrayTag final : public ArrayTagBase<int32_t, F> {
    private:
        using Base = ArrayTagBase<int32_t, F>;

    public:
        using Base::Base;

        [[nodiscard]] Nbt::TagType getType() const override {
            return Nbt::TagType::IntArray;
        }

        void write(cubix::BinaryStream& stream) override {
            this->writeImpl(stream, [](cubix::BinaryStream& s, const int32_t value) {
                s.writeInt32(value);
            });
        };

        [[nodiscard]] std::expected<void, std::runtime_error>
        read(cubix::BinaryStream& stream) override {
            return this->readImpl(stream, [](cubix::BinaryStream& s) {
                return s.tryRead<int32_t>();
            });
        }

        [[nodiscard]] std::shared_ptr<Nbt::Tag> copy() const override {
            return std::make_shared<IntArrayTag>(*this);
        }

        [[nodiscard]] std::string toString(int /*indent*/) const override {
            auto transformed = this->elements() | std::views::transform([](auto v) {
                                   return std::to_string(v) + "i";
                               });

            std::string result = "[I; ";
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

#endif // !INTARRAYTAG_HPP