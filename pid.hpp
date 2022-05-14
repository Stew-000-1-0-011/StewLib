#pragma once

namespace StewLib
{
    template<class T, class Kp = double, class Ki = double, class Kd = double>
    struct Pid
    {
        // ここを変えたいなら新しくオブジェクト作ろう
        const Kp k_p;
        const Ki k_i;
        const Kd k_d;

        T sum_dev{};  // 0にあたる値
        T last_dev{};  // 初期値どうでもいい

        T operator()(const T& dev) noexcept
        {
            sum_dev += dev;
            last_dev = dev;

            return k_p * dev + k_i * sum_dev + k_d * last_dev;
        }
    };
}