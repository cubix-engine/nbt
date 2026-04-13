#ifndef ARRAYTAGBASE_HPP
#define ARRAYTAGBASE_HPP

#include "nbt/Tag.hpp"
#include "nbt/io/NbtFormat.hpp"
#include "nbt/io/NbtIo.hpp"

namespace Nbt {

    template <typename T, NbtFormat F>
    class ArrayTagBase : public Nbt::Tag {
    private:
        using ArrayType = std::vector<T>;
        ArrayType mElements{};

    public:
        ArrayTagBase() = default;
        ArrayTagBase(const std::initializer_list<T> tags) : mElements(tags) {};

        auto begin() {
            return mElements.begin();
        };
        auto end() {
            return mElements.end();
        };
        [[nodiscard]] auto begin() const {
            return mElements.begin();
        };
        [[nodiscard]] auto end() const {
            return mElements.end();
        };

        [[nodiscard]] size_t size() const {
            return mElements.size();
        }
        [[nodiscard]] bool empty() const {
            return mElements.empty();
        }

        void clear() {
            this->mElements.clear();
        }

        [[nodiscard]] T operator[](const size_t idx) {
            return this->mElements[idx];
        }

        [[nodiscard]] T operator[](const size_t idx) const {
            return this->mElements[idx];
        }

        [[nodiscard]] T at(const size_t idx) {
            return this->mElements.at(idx);
        }

        [[nodiscard]] T at(const size_t idx) const {
            return this->mElements.at(idx);
        }

        ArrayTagBase& add(T value) {
            this->mElements.emplace_back(value);
            return *this;
        }

    protected:
        const ArrayType& elements() const {
            return mElements;
        }
        ArrayType& elements() {
            return mElements;
        }

        template <typename WriteFn>
        void writeImpl(cubix::BinaryStream& stream, WriteFn&& fn) {
            Nbt::io<F>::template writeList<T>(stream, mElements, std::forward<WriteFn>(fn));
        };

        template <typename ReadFn>
        [[nodiscard]] std::expected<void, std::runtime_error>
        readImpl(cubix::BinaryStream& stream, ReadFn&& fn) {
            auto result = Nbt::io<F>::template readList<T>(stream, std::forward<ReadFn>(fn));

            if (!result) {
                return std::unexpected{result.error()};
            }

            this->mElements = std::move(*result);
            return {};
        }
    };
} // namespace Nbt

#endif // !ARRAYTAGBASE_HPP