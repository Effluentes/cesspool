#pragma once

#include <array>
#include <cstddef>
#include <utility>

namespace configuration {
    template <std::size_t Capacity> class FixedText {
      public:
        [[nodiscard]] char *data() noexcept {
            return bytes_.data();
        }

        [[nodiscard]] char const *data() const noexcept {
            return bytes_.data();
        }

        [[nodiscard]] std::size_t capacity() const noexcept {
            return bytes_.size();
        }

        [[nodiscard]] std::size_t size() const noexcept {
            return size_;
        }

        [[nodiscard]] bool setSize(std::size_t size) noexcept {
            if (size >= bytes_.size()) {
                return false;
            }
            size_ = size;
            bytes_[size_] = '\0';
            return true;
        }

      private:
        std::array<char, Capacity> bytes_{};
        std::size_t size_{0};
    };

    class SecretText {
      public:
        SecretText() = default;
        SecretText(SecretText const &) = delete;
        SecretText &operator=(SecretText const &) = delete;

        SecretText(SecretText &&other) noexcept
            : bytes_(other.bytes_), size_(std::exchange(other.size_, 0)) {
            other.clear();
        }

        SecretText &operator=(SecretText &&other) noexcept {
            if (this != &other) {
                clear();
                bytes_ = other.bytes_;
                size_ = std::exchange(other.size_, 0);
                other.clear();
            }
            return *this;
        }

        ~SecretText() {
            clear();
        }

        [[nodiscard]] char *data() noexcept {
            return bytes_.data();
        }

        [[nodiscard]] char const *data() const noexcept {
            return bytes_.data();
        }

        [[nodiscard]] std::size_t size() const noexcept {
            return size_;
        }

        [[nodiscard]] std::size_t capacity() const noexcept {
            return bytes_.size();
        }

        [[nodiscard]] bool setSize(std::size_t size) noexcept {
            if (size >= bytes_.size()) {
                return false;
            }
            size_ = size;
            bytes_[size_] = '\0';
            return true;
        }

      private:
        void clear() noexcept {
            volatile char *bytes = bytes_.data();
            for (std::size_t index = 0; index < bytes_.size(); ++index) {
                bytes[index] = '\0';
            }
            size_ = 0;
        }

        std::array<char, 65> bytes_{};
        std::size_t size_{0};
    };

    struct WifiConfiguration {
        FixedText<33> ssid;
        SecretText password;
    };

    struct Configuration {
        WifiConfiguration wifi;
    };
} // namespace configuration
