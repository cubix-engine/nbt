#ifndef LONGARRAYTAG_HPP
#define LONGARRAYTAG_HPP

#include "ArrayTagBase.hpp"
#include "nbt/Tag.hpp"
#include "nbt/io/NbtFormat.hpp"

namespace Nbt {

    template <NbtFormat F>
    class LongArrayTag final : public ArrayTagBase<int64_t, F> {
    private:
        using Base = ArrayTagBase<int64_t, F>;

    public:
        using Base::Base;

        [[nodiscard]] Nbt::TagType getType() const override {
            return Nbt::TagType::LongArray;
        }

        void write(cubix::BinaryStream& stream) override {
            this->writeImpl(stream);
        };

        [[nodiscard]] std::expected<void, std::runtime_error>
        read(cubix::BinaryStream& stream) override {
            return this->readImpl(stream);
        }

        [[nodiscard]] std::unique_ptr<Nbt::Tag> copy() const override {
            return std::make_unique<LongArrayTag>(*this);
        }

        [[nodiscard]] std::string toString(int /*indent*/) const override {
            auto transformed = this->elements() | std::views::transform([](auto v) {
                                   return std::to_string(v) + "l";
                               });

            std::string result = "[L; ";
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

#endif // !LONGARRAYTAG_HPP