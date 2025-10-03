#pragma once

#include <iostream>
#include <memory>
#include <typeinfo>
#include <type_traits>

namespace Scheduler {

    class SomeType {

    public:

        SomeType() : content_(nullptr) {}

        SomeType(SomeType&& other) noexcept : content_(std::move(other.content_)) {}

        SomeType(const SomeType& other) : content_(other.content_ ? other.content_->MakeCopy() : nullptr) {}

        template<typename T, typename = std::enable_if_t<!std::is_same_v<T, SomeType>>>
        SomeType(const T& value) : content_(std::make_unique<Holder<T>>(std::move(value))) {}

        SomeType& operator=(const SomeType& other) {
            SomeType(std::move(other)).Swap(*this);
            return *this;
        }

        SomeType& operator=(SomeType&& other) noexcept {
            SomeType(std::move(other)).Swap(*this);
            return *this;
        }

        template<typename T, typename = std::enable_if_t<!std::is_same_v<T, SomeType>>>
        SomeType& operator=(const T& value) {
            content_ = std::make_unique<Holder<T>>(std::move(value));
            return *this;
        }

        template<typename T>
        T& Cast() {
            if (typeid(T) != content_->GetType()) {
                throw BadAnyCast("Error: Different types");
            }
            return static_cast<Holder<T>*>(content_.get())->value_;
        }

        template<typename T>
        const T& Cast() const {
            if (typeid(T) != content_->GetType()) {
                throw BadAnyCast("Error: Different types");
            }
            return static_cast<Holder<T>*>(content_.get())->value_;
        }

        void Swap(SomeType& other) { std::swap(content_, other.content_); }

    private:

        class PlaceHolder {

        public:

            virtual ~PlaceHolder() = default;

            virtual const std::type_info& GetType() const = 0;

            virtual PlaceHolder* MakeCopy() const = 0;

        };

        template<typename T>
        class Holder : public PlaceHolder {

        public:

            Holder(T value) : value_(std::move(value)) {}

            const std::type_info& GetType() const override {
                return typeid(T);
            }

            PlaceHolder* MakeCopy() const override {
                return new Holder(std::move(value_));
            }

            T value_;

        };

        class BadAnyCast : public std::exception {

        public:

            BadAnyCast(const char* msg) : message(msg) {}

            BadAnyCast(std::string&& msg) : message(std::move(msg)) {}

            const char* what() const throw() {
                return message.c_str();
            }

        private:

            std::string message;

        };

        std::unique_ptr<PlaceHolder> content_;

    };
} 