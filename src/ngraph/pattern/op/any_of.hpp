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
// WITHOUT WARRANTIES OR CONDITIONS OF AnyOf KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//*****************************************************************************

#pragma once

#include "ngraph/node.hpp"
#include "ngraph/pattern/op/pattern.hpp"

namespace ngraph
{
    namespace pattern
    {
        namespace op
        {
            /// \brief AnyOfs are used in patterns to express arbitrary queries on a node
            ///
            /// When AnyOf predicate matches a node; Matcher tries to match node's arguments to
            /// a single argument of AnyOf one by one. The first match is returned.
            /// This is useful for nodes with variable number of arguments such as Concat
            /// AnyOf enables on to specify one single branch/chain. The remaining arguments
            /// can be discovered (in a callback) by simply inspecting matched node's argument.
            class NGRAPH_API AnyOf : public Pattern
            {
            public:
                static constexpr NodeTypeInfo type_info{"patternAnyOf", 0};
                const NodeTypeInfo& get_type_info() const override;
                /// \brief creates a AnyOf node containing a sub-pattern described by \sa type and
                ///        \sa shape.
                AnyOf(const element::Type& type,
                      const PartialShape& s,
                      ValuePredicate pred,
                      const OutputVector& wrapped_values)
                    : Pattern(wrapped_values, pred)
                {
                    if (wrapped_values.size() != 1)
                    {
                        throw ngraph_error("AnyOf expects exactly one argument");
                    }
                    set_output_type(0, type, s);
                }
                AnyOf(const element::Type& type,
                      const PartialShape& s,
                      NodePredicate pred,
                      const NodeVector& wrapped_values)
                    : AnyOf(type,
                            s,
                            [pred](const Output<Node>& value) {
                                return pred(value.as_single_output_node(false));
                            },
                            as_output_vector(wrapped_values))
                {
                }

                /// \brief creates a AnyOf node containing a sub-pattern described by the type and
                ///        shape of \sa node.
                AnyOf(const Output<Node>& node,
                      ValuePredicate pred,
                      const OutputVector& wrapped_values)
                    : AnyOf(node.get_element_type(), node.get_partial_shape(), pred, wrapped_values)
                {
                }
                AnyOf(std::shared_ptr<Node> node,
                      NodePredicate pred,
                      const NodeVector& wrapped_values)
                    : AnyOf(node, as_value_predicate(pred), as_output_vector(wrapped_values))
                {
                }
                bool match_value(Matcher& matcher,
                                 const Output<Node>& pattern_value,
                                 const Output<Node>& graph_value,
                                 PatternValueMap& pattern_map) override;
            };
        }
    }
}
