#pragma once

#include <concepts>
#include <expected>

namespace runtime::application
{
    enum class UserInput
    {
        rotateLeft,
        rotateRight,
        select,
        confirm,
        back
    };

    enum class InputError
    {
        hardwareInitializationFailed,
        resourceUnavailable
    };

    template <typename T>
    concept InputReceiver = requires(T &receiver, UserInput input)
    {
        { receiver.post(input) } noexcept -> std::same_as<void>;
    };

    struct InputSink
    {
        void *context;
        void (*postFunction)(void *, UserInput) noexcept;

        template <InputReceiver Receiver>
        [[nodiscard]] static InputSink bind(Receiver &receiver) noexcept
        {
            return {
                &receiver,
                [](void *context, UserInput input) noexcept
                {
                    static_cast<Receiver *>(context)->post(input);
                }};
        }

        void post(UserInput input) const noexcept
        {
            postFunction(context, input);
        }
    };

    template <typename T>
    concept InputAdapter = requires(T &adapter, InputSink const &sink)
    {
        { adapter.start(sink) } -> std::same_as<std::expected<void, InputError>>;
    };

    struct InputPort
    {
        void *context;
        std::expected<void, InputError> (*startFunction)(void *, InputSink const &);

        template <InputAdapter Adapter>
        [[nodiscard]] static InputPort bind(Adapter &adapter) noexcept
        {
            return {
                &adapter,
                [](void *context, InputSink const &sink)
                {
                    return static_cast<Adapter *>(context)->start(sink);
                }};
        }

        [[nodiscard]] std::expected<void, InputError> start(InputSink const &sink) const
        {
            return startFunction(context, sink);
        }
    };
}
