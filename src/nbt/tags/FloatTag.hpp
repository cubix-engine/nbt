#ifndef FLOATTAG_HPP
#define FLOATTAG_HPP

#include "nbt/Tag.hpp"
#include "nbt/io/NbtFormat.hpp"
#include "nbt/io/NbtIo.hpp"

namespace Nbt {

    template <NbtFormat F>
    class FloatTag final : public Nbt::Tag {
    private:
        float mValue{0};

    public:
        explicit FloatTag(const float value = 0) : mValue(value) {};

        [[nodiscard]] Nbt::TagType getType() const override {
            return Nbt::TagType::Float;
        }

        [[nodiscard]] float getValue() const {
            return this->mValue;
        }

        void setValue(const float value) {
            this->mValue = value;
        }

        void write(cubix::BinaryStream& stream) override {
            const auto value = this->getValue();
            Nbt::io<F>::template writeInt<float>(stream, value);
        };

        [[nodiscard]] std::expected<void, std::runtime_error>
        read(cubix::BinaryStream& stream) override {
            const auto value = Nbt::io<F>::template readInt<float>(stream);
            if (!value) {
                return std::unexpected(value.error());
            }

            this->setValue(*value);
            return {};
        }

        [[nodiscard]] std::shared_ptr<Nbt::Tag> copy() const override {
            return std::make_shared<FloatTag>(*this);
        }

        [[nodiscard]] std::string toString(int /*indent*/) const override {
            return std::to_string(this->mValue) + "f";
        }
    };
} // namespace Nbt

#endif // !FLOATTAG_HPP