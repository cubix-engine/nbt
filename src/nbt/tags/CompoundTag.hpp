#ifndef COMPOUNDTAG_HPP
#define COMPOUNDTAG_HPP
#include <algorithm>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "nbt/Tag.hpp"
#include "nbt/io/NbtFormat.hpp"
#include "nbt/io/NbtWriter.hpp"

#include "ByteTag.hpp"
#include "DoubleTag.hpp"
#include "FloatTag.hpp"
#include "Int64Tag.hpp"
#include "IntArrayTag.hpp"
#include "IntTag.hpp"
#include "ListTag.hpp"
#include "LongArrayTag.hpp"
#include "ShortTag.hpp"
#include "StringTag.hpp"

namespace Nbt {

    template <NbtFormat F>
    std::expected<std::pair<std::string, std::unique_ptr<Tag>>, std::runtime_error>
    readNamed(cubix::BinaryStream&);

    template <NbtFormat F>
    class CompoundTag;

    template <typename T>
    struct TagTypeOf;

    template <NbtFormat F>
    struct TagTypeOf<ByteTag<F>> {
        static constexpr auto value = TagType::Byte;
    };

    template <NbtFormat F>
    struct TagTypeOf<ShortTag<F>> {
        static constexpr auto value = TagType::Short;
    };

    template <NbtFormat F>
    struct TagTypeOf<IntTag<F>> {
        static constexpr auto value = TagType::Int;
    };

    template <NbtFormat F>
    struct TagTypeOf<Int64Tag<F>> {
        static constexpr auto value = TagType::Int64;
    };

    template <NbtFormat F>
    struct TagTypeOf<FloatTag<F>> {
        static constexpr auto value = TagType::Float;
    };

    template <NbtFormat F>
    struct TagTypeOf<DoubleTag<F>> {
        static constexpr auto value = TagType::Double;
    };

    template <NbtFormat F>
    struct TagTypeOf<StringTag<F>> {
        static constexpr auto value = TagType::String;
    };

    template <NbtFormat F>
    struct TagTypeOf<ListTag<F>> {
        static constexpr auto value = TagType::List;
    };

    template <NbtFormat F>
    struct TagTypeOf<CompoundTag<F>> {
        static constexpr auto value = TagType::Compound;
    };

    template <NbtFormat F>
    struct TagTypeOf<ByteArrayTag<F>> {
        static constexpr auto value = TagType::ByteArray;
    };

    template <NbtFormat F>
    struct TagTypeOf<IntArrayTag<F>> {
        static constexpr auto value = TagType::IntArray;
    };

    template <NbtFormat F>
    struct TagTypeOf<LongArrayTag<F>> {
        static constexpr auto value = TagType::LongArray;
    };

    template <NbtFormat F>
    class CompoundTag final : public Nbt::Tag {
    private:
        using TagPair = std::pair<std::string, std::unique_ptr<Tag>>;
        using TagList = std::vector<TagPair>;

        TagList                                 mTags{};
        std::unordered_map<std::string, size_t> mLookup{};

    public:
        using iterator       = TagList::iterator;
        using const_iterator = TagList::const_iterator;
        using size_type      = TagList::size_type;

        CompoundTag() {
            mTags.reserve(32);
            mLookup.reserve(32);
        }
        ~CompoundTag() override = default;

        CompoundTag(const CompoundTag&)            = delete;
        CompoundTag& operator=(const CompoundTag&) = delete;

        CompoundTag(CompoundTag&&) noexcept            = default;
        CompoundTag& operator=(CompoundTag&&) noexcept = default;

        // Container API

        void reserve(const size_t count) {
            mTags.reserve(count);
            mLookup.reserve(count);
        }

        [[nodiscard]] size_type size() const noexcept {
            return mTags.size();
        }
        [[nodiscard]] bool empty() const noexcept {
            return mTags.empty();
        }

        void clear() noexcept {
            mTags.clear();
            mLookup.clear();
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

        CompoundTag& add(std::string name, const Tag& tag) {
            auto index = mTags.size();

            mLookup.emplace(name, index);
            mTags.emplace_back(std::move(name), tag.copy());

            return *this;
        }

        template <typename T>
            requires std::derived_from<T, Tag>
        CompoundTag& add(std::string name, std::unique_ptr<T> tag) {
            auto index = mTags.size();

            mLookup.emplace(name, index);
            mTags.emplace_back(std::move(name), std::move(tag));

            return *this;
        }

        template <typename T, typename... Args>
            requires std::derived_from<T, Tag>
        T& emplace(std::string name, Args&&... args) {
            auto ptr = std::make_unique<T>(std::forward<Args>(args)...);

            T& ref = *ptr;

            auto index = mTags.size();

            mLookup.emplace(name, index);
            mTags.emplace_back(std::move(name), std::move(ptr));

            return ref;
        }

        // Lookup

        iterator find(const std::string& key) {
            return findImpl(key);
        }

        [[nodiscard]] const_iterator find(const std::string& key) const {
            return findImpl(key);
        }

        [[nodiscard]] bool contains(const std::string& key) const {
            return mLookup.contains(key);
        }

        std::unique_ptr<Tag>& at(const std::string& key) {
            auto it = findImpl(key);
            if (it == mTags.end()) {
                throw std::out_of_range("Tag not found: " + key);
            }

            return it->second;
        }

        [[nodiscard]] const std::unique_ptr<Tag>& at(const std::string& key) const {
            auto it = findImpl(key);
            if (it == mTags.end()) {
                throw std::out_of_range("Tag not found: " + key);
            }

            return it->second;
        }

        std::unique_ptr<Tag>& operator[](const std::string& key) {
            auto it = findImpl(key);
            if (it != mTags.end()) {
                return it->second;
            }

            auto index = mTags.size();

            mLookup.emplace(key, index);
            mTags.emplace_back(key, nullptr);

            return mTags.back().second;
        }

        size_type erase(const std::string& key) {
            auto it = findImpl(key);
            if (it == mTags.end()) {
                return 0;
            }

            mTags.erase(it);

            rebuildLookup();
            return 1;
        }

        // Typed access

        template <typename T>
        T* getAs(const std::string& key) {
            auto it = findImpl(key);
            if (it == mTags.end()) {
                return nullptr;
            }

            auto* tag = it->second.get();

            if (tag->getType() != TagTypeOf<T>::value) {
                return nullptr;
            }

            return static_cast<T*>(tag);
        }

        template <typename T>
        const T* getAs(const std::string& key) const {
            auto it = findImpl(key);
            if (it == mTags.end()) {
                return nullptr;
            }

            auto* tag = it->second.get();

            if (tag->getType() != TagTypeOf<T>::value) {
                return nullptr;
            }

            return static_cast<const T*>(tag);
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

        CompoundTag* getCompound(const std::string& key) {
            return getAs<CompoundTag>(key);
        }

        const CompoundTag* getCompound(const std::string& key) const {
            return getAs<CompoundTag>(key);
        }

        // Tag interface

        [[nodiscard]] TagType getType() const override {
            return TagType::Compound;
        }

        [[nodiscard]] std::unique_ptr<Tag> copy() const override {
            auto result = std::make_unique<CompoundTag>();

            result->mTags.reserve(mTags.size());
            result->mLookup.reserve(mTags.size());

            for (const auto& [name, tag] : mTags) {
                auto index = result->mTags.size();

                result->mLookup.emplace(name, index);
                result->mTags.emplace_back(name, tag ? tag->copy() : nullptr);
            }

            return result;
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

                auto&& [name, tag] = *named;
                if (tag == nullptr || tag->getType() == TagType::End) {
                    break;
                }

                this->add(std::move(name), std::move(tag));
            }

            return {};
        }

        // Debug / SNBT

        [[nodiscard]] std::string toString(const int indent) const override {
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
        void rebuildLookup() {
            mLookup.clear();
            mLookup.reserve(mTags.size());

            for (size_t i = 0; i < mTags.size(); ++i) {
                mLookup.emplace(mTags[i].first, i);
            }
        }

        // Centralized lookup
        auto findImpl(const std::string& key) {
            const auto lookup = mLookup.find(key);

            if (lookup == mLookup.end()) {
                return mTags.end();
            }

            return mTags.begin() + static_cast<std::ptrdiff_t>(lookup->second);
        }

        auto findImpl(const std::string& key) const {
            const auto lookup = mLookup.find(key);

            if (lookup == mLookup.end()) {
                return mTags.end();
            }

            return mTags.begin() + static_cast<std::ptrdiff_t>(lookup->second);
        }
    };

} // namespace Nbt

#endif // COMPOUNDTAG_HPP