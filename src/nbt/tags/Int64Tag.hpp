#ifndef INT64TAG_HPP
#define INT64TAG_HPP

#include "nbt/Tag.hpp"
#include "nbt/io/NbtFormat.hpp"
#include "nbt/io/NbtIo.hpp"

namespace Nbt {

    template <NbtFormat F>
    class Int64Tag final : public Nbt::Tag {
    private:
        int64_t mValue{0};

    public:
        explicit Int64Tag(const int64_t value = 0) : mValue(value) {};

        [[nodiscard]] Nbt::TagType getType() const override {
            return Nbt::TagType::Int64;
        }

        [[nodiscard]] int64_t getValue() const {
            return this->mValue;
        }

        void setValue(const int64_t value) {
            this->mValue = value;
        }

        void write(cubix::BinaryStream& stream) override {
            const auto value = this->getValue();
            Nbt::io<F>::template writeInt<int64_t>(stream, value);
        }

        [[nodiscard]] std::expected<void, std::runtime_error>
        read(cubix::BinaryStream& stream) override {
            const auto value = Nbt::io<F>::template readInt<int64_t>(stream);
            if (!value) {
                return std::unexpected(value.error());
            }

            this->setValue(*value);
            return {};
        }

        [[nodiscard]] std::unique_ptr<Nbt::Tag> copy() const override {
            return std::make_unique<Int64Tag>(*this);
        }

        [[nodiscard]] std::string toString(int /*indent*/) const override {
            return std::to_string(this->mValue) + "l";
        }
    };
} // namespace Nbt

#endif // !INT64TAG_HPP