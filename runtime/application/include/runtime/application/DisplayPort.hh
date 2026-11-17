#pragma once

#include <runtime/application/Application.hh>

#include <concepts>

namespace runtime::application
{
    template <typename T>
    concept DisplayRenderer = requires(T &renderer, View const &view)
    {
        { renderer.render(view) } -> std::same_as<void>;
    };

    struct DisplayPort
    {
        void *context;
        void (*renderFunction)(void *, View const &);

        template <DisplayRenderer Renderer>
        [[nodiscard]] static DisplayPort bind(Renderer &renderer) noexcept
        {
            return {
                &renderer,
                [](void *context, View const &view)
                {
                    static_cast<Renderer *>(context)->render(view);
                }};
        }

        void render(View const &view) const
        {
            renderFunction(context, view);
        }
    };
}
