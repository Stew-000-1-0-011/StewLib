#pragma once

#include <vector>
#include <algorithm>
#include <execution>

#include "../parallel.hpp"
#include "vec2d.hpp"
#include "line.hpp"

namespace StewLib
{
    namespace
    {
        namespace Math
        {
            // 頂点は必ず2個以上にすること。
            template<class T>
            struct Polygon final
            {
                Vec2D<T> center{};
                std::vector<Vec2D<T>> vertexs{};

                constexpr bool is_in(const low_cost_ref_val_t<Vec2D<T>> point) const noexcept
                {
                    // 以下ローカルな座標系で考える。
                    const Line<T> judge_line{{}, {}, point, LineType::line};

                    const auto edges = this->edges();

                    {
                        auto pred=[&judge_line](const auto edge) noexcept
                        {
                            return is_cross_l(judge_line, edge);
                        };

                        return std::count_if(std::execution::par_unseq, edges.cbegin(), edges.cend(), pred);
                    }
                }

                constexpr std::vector<Line<T>> edges() const noexcept
                {
                    const auto vertexs_size = vertexs.size();
                    const auto center = this->center;
                    std::vector<Line<T>> ret(vertexs_size);

                    ret[0] = Line<T>{center, vertexs[vertexs_size - 1], vertexs[0]};
                    
                    {
                        auto for_body=[&ret, &vertexs](const auto i) noexcept
                        {
                            ret[i] = Line<T>{center, vertexs[i], vertexs[i + 1]};
                        };

                        for_i(std::execution::par_unseq, vertexs_size, for_body);
                    }

                    return ret;
                }
            };
        }
    }
}