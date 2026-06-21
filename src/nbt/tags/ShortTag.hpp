#ifndef SHORTTAG_HPP
#define SHORTTAG_HPP

#include "nbt/Tag.hpp"
#include "nbt/io/NbtFormat.hpp"
#include "nbt/io/NbtIo.hpp"

namespace Nbt {

    template <NbtFormat F>
    class ShortTag final : public Nbt::Tag {
    private:
        int16_t mValue{0};

    public:
        explicit ShortTag(const int16_t value = 0) : mValue(value) {};

        [[nodiscard]] Nbt::TagType getType() const override {
            return Nbt::TagType::Short;
        }

        [[nodiscard]] int16_t getValue() const {
            return this->mValue;
        }

        void setValue(const int16_t value) {
            this->mValue = value;
        }

        void write(cubix::BinaryStream& stream) override {
            const auto value = this->getValue();
            Nbt::io<F>::template writeInt<int16_t>(stream, value);
        }

        [[nodiscard]] std::expected<void, std::runtime_error>
        read(cubix::BinaryStream& stream) override {
            const auto value = Nbt::io<F>::template readInt<int16_t>(stream);
            if (!value) {
                return std::unexpected(value.error());
            }

            this->setValue(*value);
            return {};
        }

        [[nodiscard]] std::unique_ptr<Nbt::Tag> copy() const override {
            return std::make_unique<ShortTag>(*this);
        }

        [[nodiscard]] std::string toString(int /*indent*/) const override {
            return std::to_string(this->mValue) + "s";
        }
    };
} // namespace Nbt

#endif // !SHORTTAG_HPP