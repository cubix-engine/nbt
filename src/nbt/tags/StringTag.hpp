#ifndef STRINGTAG_HPP
#define STRINGTAG_HPP

#include "nbt/Tag.hpp"
#include "nbt/io/NbtFormat.hpp"
#include "nbt/io/NbtIo.hpp"

namespace Nbt {

    template <NbtFormat F>
    class StringTag final : public Nbt::Tag {
    private:
        std::string mValue{};

    public:
        StringTag() = default;
        explicit StringTag(const std::string& value) : mValue(value) {};

        [[nodiscard]] Nbt::TagType getType() const override {
            return Nbt::TagType::String;
        }

        [[nodiscard]] const std::string& getValue() const {
            return this->mValue;
        }

        void setValue(const std::string& value) {
            this->mValue = value;
        }

        void write(cubix::BinaryStream& stream) override {
            const auto value = this->getValue();
            Nbt::io<F>::writeString(stream, value);
        }

        [[nodiscard]] std::expected<void, std::runtime_error>
        read(cubix::BinaryStream& stream) override {
            const auto value = Nbt::io<F>::readString(stream);
            if (!value) {
                return std::unexpected(value.error());
            }

            this->setValue(*value);
            return {};
        }

        [[nodiscard]] std::unique_ptr<Nbt::Tag> copy() const override {
            return std::make_unique<StringTag>(*this);
        }

        [[nodiscard]] std::string toString(int /*indent*/) const override {
            return std::format("'{}'", this->mValue);
        }
    };
} // namespace Nbt

#endif // !STRINGTAG_HPP