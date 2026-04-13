#ifndef COMPOUNDTAG_HPP
#define COMPOUNDTAG_HPP
#include <algorithm>
#include <memory>
#include <string>
#include <vector>

#include "nbt/Tag.hpp"
#include "nbt/io/NbtFormat.hpp"
#include "nbt/io/NbtWriter.hpp"

#include "ByteTag.hpp"
#include "IntTag.hpp"

namespace Nbt {
    template <NbtFormat F>
    std::expected<std::pair<std::string, std::shared_ptr<Tag>>, std::runtime_error>
    readNamed(cubix::BinaryStream&);
}

namespace Nbt {

    template <NbtFormat F>
    class CompoundTag final : public Nbt::Tag {
    private:
        using TagPair = std::pair<std::string, std::shared_ptr<Tag>>;
        using TagList = std::vector<TagPair>;

        TagList mTags{};

    public:
        using iterator       = TagList::iterator;
        using const_iterator = TagList::const_iterator;
        using size_type      = TagList::size_type;

        CompoundTag()           = default;
        ~CompoundTag() override = default;

        // Container API

        [[nodiscard]] size_type size() const noexcept {
            return mTags.size();
        }
        [[nodiscard]] bool empty() const noexcept {
            return mTags.empty();
        }
        void clear() noexcept {
            mTags.clear();
        }

        iterator begin() noexcept {
            return mTags.begin();
        }
        iterator end() noexcept {
            return mTags.end();
        }

        [[nodiscard]] const_iterator begin() const noexcept {
            return mTags.begin();
        }
        [[nodiscard]] const_iterator end() const noexcept {
            return mTags.end();
        }

        // Insertion

        template <typename T>
            requires std::derived_from<std::decay_t<T>, Tag>
        CompoundTag& add(std::string name, T&& tag) {
            mTags.emplace_back(
                std::move(name), std::make_shared<std::decay_t<T>>(std::forward<T>(tag))
            );
            return *this;
        }

        CompoundTag& add(std::string name, std::shared_ptr<Tag> tag) {
            mTags.emplace_back(std::move(name), std::move(tag));
            return *this;
        }

        // Lookup

        iterator find(const std::string& key) {
            return findImpl(key);
        }

        [[nodiscard]] const_iterator find(const std::string& key) const {
            return findImpl(key);
        }

        [[nodiscard]] bool contains(const std::string& key) const {
            return findImpl(key) != mTags.end();
        }

        std::shared_ptr<Tag>& at(const std::string& key) {
            auto it = findImpl(key);
            if (it == mTags.end()) {
                throw std::out_of_range("Tag not found: " + key);
            }

            return it->second;
        }

        [[nodiscard]] const std::shared_ptr<Tag>& at(const std::string& key) const {
            auto it = findImpl(key);
            if (it == mTags.end()) {
                throw std::out_of_range("Tag not found: " + key);
            }

            return it->second;
        }

        std::shared_ptr<Tag>& operator[](const std::string& key) {
            auto it = findImpl(key);
            if (it != mTags.end()) {
                return it->second;
            }

            mTags.emplace_back(key, nullptr);
            return mTags.back().second;
        }

        size_type erase(const std::string& key) {
            auto it = findImpl(key);
            if (it == mTags.end()) {
                return 0;
            }

            mTags.erase(it);
            return 1;
        }

        // Typed access

        template <typename T>
        T* getAs(const std::string& key) {
            if (auto it = findImpl(key); it != mTags.end()) {
                return dynamic_cast<T*>(it->second.get());
            }

            return nullptr;
        }

        template <typename T>
        const T* getAs(const std::string& key) const {
            if (auto it = findImpl(key); it != mTags.end()) {
                return dynamic_cast<const T*>(it->second.get());
            }

            return nullptr;
        }

        template <typename T>
        const T& require(const std::string& key) const {
            auto ptr = getAs<T>(key);
            if (!ptr) {
                throw std::runtime_error("Invalid type for key: " + key);
            }

            return *ptr;
        }

        // Helpers

        [[nodiscard]] int32_t getInt(const std::string& key) const {
            return require<IntTag>(key).getValue();
        }

        [[nodiscard]] int8_t getByte(const std::string& key) const {
            return require<ByteTag>(key).getValue();
        }

        std::shared_ptr<CompoundTag> getCompound(const std::string& key) const {
            return std::static_pointer_cast<CompoundTag>(at(key));
        }

        // Tag interface

        [[nodiscard]] TagType getType() const override {
            return TagType::Compound;
        }

        [[nodiscard]] std::shared_ptr<Tag> copy() const override {
            return std::make_shared<CompoundTag>(*this);
        }

        void write(cubix::BinaryStream& stream) override {
            for (auto& [key, value] : mTags) {
                Nbt::writeNamed<F>(stream, key, value);
            }

            // Write end tag
            stream.writeInt8(static_cast<int8_t>(Nbt::TagType::End));
        };

        [[nodiscard]] std::expected<void, std::runtime_error>
        read(cubix::BinaryStream& stream) override {
            while (true) {
                auto named = Nbt::readNamed<F>(stream);
                if (!named) {
                    return std::unexpected(named.error());
                }

                auto [name, tag] = *named;
                if (tag == nullptr || tag->getType() == TagType::End) {
                    break;
                }

                this->add(name, tag);
            }

            return {};
        }

        // Debug / SNBT

        std::string toString(const int indent) const override {
            if (this->empty()) {
                return "{}";
            }

            const std::string indentString(static_cast<size_t>(indent), ' ');
            const std::string nextIndent(static_cast<size_t>(indent + 2), ' ');

            std::string result = "{\n";
            bool        first  = true;

            for (const auto& [key, value] : mTags) {
                if (!value || value->getType() == TagType::End)
                    continue;

                if (!first) {
                    result += ",\n";
                }

                result += nextIndent + key + ": " + value->toString(indent + 2);
                first   = false;
            }

            result += "\n" + indentString + "}";
            return result;
        }

    private:
        // Centralized lookup
        auto findImpl(const std::string& key) {
            return std::ranges::find_if(mTags, [&](const auto& p) { return p.first == key; });
        }

        auto findImpl(const std::string& key) const {
            return std::ranges::find_if(mTags, [&](const auto& p) { return p.first == key; });
        }
    };

} // namespace Nbt

#endif // COMPOUNDTAG_HPP