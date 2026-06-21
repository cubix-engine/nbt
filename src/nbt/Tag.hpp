#ifndef TAG_HPP
#define TAG_HPP
#include <memory>
#include <BinaryStream/BinaryStream.hpp>

namespace Nbt {
    enum class TagType : uint8_t {
        End       = 0x00,
        Byte      = 0x01,
        Short     = 0x02,
        Int       = 0x03,
        Int64     = 0x04,
        Float     = 0x05,
        Double    = 0x06,
        ByteArray = 0x07,
        String    = 0x08,
        List      = 0x09,
        Compound  = 0x0A,
        IntArray  = 0x0B,
        LongArray = 0x0C
    };

    class Tag {
    public:
        virtual ~Tag() = default;

        [[nodiscard]] virtual Nbt::TagType         getType() const = 0;
        [[nodiscard]] virtual std::unique_ptr<Tag> copy() const    = 0;

        virtual void                                    write(cubix::BinaryStream&) = 0;
        virtual std::expected<void, std::runtime_error> read(cubix::BinaryStream&)  = 0;

        [[nodiscard]] virtual std::string toString(int indent = 0) const = 0;

    public:
        template <std::ranges::input_range R>
        static std::string join(R&& range, const std::string_view sep) {
            std::string result;

            auto it = range.begin();
            if (it != range.end()) {
                result += *it;
                ++it;
            }

            for (; it != range.end(); ++it) {
                result += sep;
                result += *it;
            }

            return result;
        }
    };

    // End Tag
    class EndTag final : public Nbt::Tag {
    public:
        EndTag() = default;

        [[nodiscard]] Nbt::TagType getType() const override {
            return Nbt::TagType::End;
        };
        [[nodiscard]] std::unique_ptr<Nbt::Tag> copy() const override {
            return std::make_unique<EndTag>(*this);
        };

        void                                    write(cubix::BinaryStream&) override {};
        std::expected<void, std::runtime_error> read(cubix::BinaryStream&) override {
            return {};
        };

        [[nodiscard]] std::string toString(int /*indent*/) const override {
            return "";
        };
    };
}; // namespace Nbt

#endif // !TAG_HPP
