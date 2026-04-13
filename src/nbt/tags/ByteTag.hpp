#ifndef BYTETAG_HPP
#define BYTETAG_HPP

#include "nbt/Tag.hpp"
#include "nbt/io/NbtFormat.hpp"
#include "nbt/io/NbtIo.hpp"

namespace Nbt {

    template <NbtFormat F>
    class ByteTag final : public Nbt::Tag {
    private:
        int8_t mValue{0};

    public:
        explicit ByteTag(const int8_t value = 0) : mValue(value) {};

        [[nodiscard]] Nbt::TagType getType() const override {
            return Nbt::TagType::Byte;
        }

        [[nodiscard]] int8_t getValue() const {
            return this->mValue;
        }

        void setValue(const int8_t value) {
            this->mValue = value;
        }

        void write(cubix::BinaryStream& stream) override {
            const auto value = this->getValue();
            Nbt::io<F>::template writeInt<int8_t>(stream, value);
        };

        [[nodiscard]] std::expected<void, std::runtime_error>
        read(cubix::BinaryStream& stream) override {
            const auto value = Nbt::io<F>::template readInt<int8_t>(stream);
            if (!value) {
                return std::unexpected(value.error());
            }

            this->setValue(*value);
            return {};
        }

        [[nodiscard]] std::shared_ptr<Nbt::Tag> copy() const override {
            return std::make_shared<ByteTag>(*this);
        }

        [[nodiscard]] std::string toString(int /*indent*/) const override {
            return std::to_string(this->mValue) + "b";
        }
    };
} // namespace Nbt

#endif // !BYTETAG_HPP