#ifndef DOUBLETAG_HPP
#define DOUBLETAG_HPP

#include "nbt/Tag.hpp"
#include "nbt/io/NbtFormat.hpp"
#include "nbt/io/NbtIo.hpp"

namespace Nbt {

    template <NbtFormat F>
    class DoubleTag final : public Nbt::Tag {
    private:
        double mValue{0};

    public:
        explicit DoubleTag(const double value = 0) : mValue(value) {};

        [[nodiscard]] Nbt::TagType getType() const override {
            return Nbt::TagType::Double;
        }

        [[nodiscard]] double getValue() const {
            return this->mValue;
        }

        void setValue(const double value) {
            this->mValue = value;
        }

        void write(cubix::BinaryStream& stream) override {
            const auto value = this->getValue();
            Nbt::io<F>::template writeInt<double>(stream, value);
        };

        [[nodiscard]] std::expected<void, std::runtime_error>
        read(cubix::BinaryStream& stream) override {
            const auto value = Nbt::io<F>::template readInt<double>(stream);
            if (!value) {
                return std::unexpected(value.error());
            }

            this->setValue(*value);
            return {};
        }

        [[nodiscard]] std::shared_ptr<Nbt::Tag> copy() const override {
            return std::make_shared<DoubleTag>(*this);
        }

        [[nodiscard]] std::string toString(int /*indent*/) const override {
            return std::to_string(this->mValue) + "d";
        }
    };
} // namespace Nbt

#endif // !DOUBLETAG_HPP