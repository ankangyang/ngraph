//*****************************************************************************
// Copyright 2017-2019 Intel Corporation
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//*****************************************************************************

#include "gtest/gtest.h"
#include "ngraph/ngraph.hpp"
#include "util/type_prop.hpp"

using namespace std;
using namespace ngraph;

TEST(type_prop, shape_of)
{
    auto a = make_shared<op::Parameter>(element::f32, Shape{1, 2, 3, 4});
    auto so = make_shared<op::ShapeOf>(a);

    ASSERT_EQ(so->get_output_element_type(0), element::i64);
    ASSERT_EQ(so->get_shape(), Shape{4});
}

TEST(type_prop, shape_of_partial_et_dynamic)
{
    auto a = make_shared<op::Parameter>(element::dynamic, Shape{1, 2, 3, 4});
    auto so = make_shared<op::ShapeOf>(a);

    ASSERT_EQ(so->get_output_element_type(0), element::i64);
    ASSERT_EQ(so->get_shape(), Shape{4});
}

TEST(type_prop, shape_of_partial_rank_static_dynamic)
{
    auto a = make_shared<op::Parameter>(
        element::f32, PartialShape{1, Dimension::dynamic(), Dimension::dynamic(), 4});
    auto so = make_shared<op::ShapeOf>(a);

    ASSERT_EQ(so->get_output_element_type(0), element::i64);
    ASSERT_EQ(so->get_shape(), Shape{4});
}

TEST(type_prop, shape_of_partial_rank_dynamic)
{
    auto a = make_shared<op::Parameter>(element::f32, PartialShape::dynamic());
    auto so = make_shared<op::ShapeOf>(a);

    ASSERT_EQ(so->get_output_element_type(0), element::i64);
    ASSERT_TRUE(so->get_output_partial_shape(0).same_scheme(PartialShape::dynamic(1)));
}
