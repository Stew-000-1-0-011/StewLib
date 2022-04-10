#pragma once

namespace StewLib
{
    namespace
    {
        class NonblockingKeyInputManager final
        {
            bool is_any_pushed_now{};
            bool is_any_pushed_once{};

            bool is_pushed_now[128];
            bool 
        };
    }
}