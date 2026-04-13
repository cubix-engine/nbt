#ifndef LISTTAG_HPP
#define LISTTAG_HPP
#include <ranges>

#include "nbt/Tag.hpp"
#include "nbt/io/NbtWriter.hpp"
#include "nbt/io/NbtFormat.hpp"
#include "nbt/io/NbtIo.hpp"

namespace Nbt {
    template <NbtFormat F>
    std::expected<std::shared_ptr<Tag>, std::runtime_error>
    readPayload(cubix::BinaryStream&, TagType type);
}

namespace Nbt {

    template <NbtFormat F>
    class ListTag final : public Nbt::Tag {
        using ListType = std::vector<std::shared_ptr<Tag>>;

        TagType  mType{TagType::End};
        ListType mElements{};

    public:
        using iterator       = ListType::iterator;
        using const_iterator = ListType::const_iterator;

        // Container API

        [[nodiscard]] size_t size() const noexcept {
            return mElements.size();
        }
        [[nodiscard]] bool empty() const noexcept {
            return mElements.empty();
        }

        void clear() noexcept {
            mElements.clear();
            mType = TagType::End;
        }

        iterator begin() noexcept {
            return mElements.begin();
        }
        iterator end() noexcept {
            return mElements.end();
        }

        [[nodiscard]] const_iterator begin() const noexcept {
            return mElements.begin();
        }
        [[nodiscard]] const_iterator end() const noexcept {
            return mElements.end();
        }

        // Access

        std::shared_ptr<Tag>& operator[](const size_t i) {
            return mElements[i];
        }
        const std::shared_ptr<Tag>& operator[](const size_t i) const {
            return mElements[i];
        }

        // Add

        ListTag& add(std::shared_ptr<Tag> tag) {
            validate(tag);
            mElements.emplace_back(std::move(tag));
            return *this;
        }

        template <typename T>
            requires std::derived_from<std::decay_t<T>, Tag>
        ListTag& add(T&& tag) {
            return add(std::make_shared<std::decay_t<T>>(std::forward<T>(tag)));
        }

        void write(cubix::BinaryStream& stream) override {
            stream.writeInt8(static_cast<int8_t>(mType));

            Nbt::io<F>::writeList(
                stream, mElements, [](cubix::BinaryStream& s, const std::shared_ptr<Tag>& tag) {
                    Nbt::writePayload(s, tag);
                }
            );
        };

        [[nodiscard]] std::expected<void, std::runtime_error>
        read(cubix::BinaryStream& stream) override {
            const auto value = stream.tryRead<int8_t>();
            if (!value) {
                return std::unexpected(value.error());
            }

            this->mType = static_cast<Nbt::TagType>(*value);
            auto result = Nbt::io<F>::template readList<std::shared_ptr<Tag>>(
                stream,
                [&](cubix::BinaryStream& s) { return Nbt::readPayload<F>(s, this->mType); }
            );

            if (!result) {
                return std::unexpected{result.error()};
            }

            this->mElements = std::move(*result);
            return {};
        }

        [[nodiscard]] TagType getListType() const {
            return mType;
        }

        [[nodiscard]] TagType getType() const override {
            return TagType::List;
        }

        [[nodiscard]] std::shared_ptr<Tag> copy() const override {
            return std::make_shared<ListTag>(*this);
        }

        [[nodiscard]] std::string toString(int /*indent*/) const override {
            auto view =
                mElements | std::views::filter([](const auto& v) {
                    return v && v->getType() != TagType::End;
                }) |
                std::ranges::views::transform([](const auto& v) { return v->toString(); });

            return "[" + Nbt::Tag::join(view, ", ") + "]";
        }

    private:
        void validate(const std::shared_ptr<Tag>& tag) {
            if (!tag) {
                throw std::runtime_error("Null tag in Nbt::ListTag");
            }

            if (mType == TagType::End) {
                mType = tag->getType();
            }
            else if (mType != tag->getType()) {
                throw std::runtime_error("Nbt::ListTag type mismatch");
            }
        }
    };
} // namespace Nbt

#endif // LISTTAG_HPP
