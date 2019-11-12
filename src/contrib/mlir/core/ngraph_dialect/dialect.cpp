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

// NOTE: This file follows nGraph format style and MLIR naming convention since it does
// not expose public API to the rest of nGraph codebase and heavily depends on MLIR API.

#include "dialect.hpp"
#include "ngraph/check.hpp"
#include "ops.hpp"
#include "type.hpp"
#include <mlir/IR/DialectImplementation.h>
#include <mlir/Parser.h>

using namespace mlir;

NGraphOpsDialect::NGraphOpsDialect(mlir::MLIRContext* ctx)
    : mlir::Dialect(getDialectNamespace(), ctx)
{
    addTypes<NGTensorType>();
    addTypes<NGIntegerType>();
    addTypes<NGBoolType>();

    addOperations<
#define GET_OP_LIST
#include "ops.cpp.inc"
        >();
}

mlir::Type NGraphOpsDialect::parseType(mlir::DialectAsmParser &parser) const
{
    MLIRContext* context = getContext();

    // Process nGraph tensor type.
    if (parser.parseOptionalKeyword("ntensor"))
    {
        llvm::SMLoc typeLoc = parser.getCurrentLocation();
        if (!parser.parseLess())
        {
            parser.emitError(typeLoc, "expected '<' and '>' enclosing the tensor shape");
            return Type();
        }

        // Parse shape dimensions.
        SmallVector<int64_t, 4> shape;
        parser.parseDimensionList(shape);

        // Parse the current element type.
        Type eltType;
        if (parser.parseOptionalKeyword("ng."))
        {
            // ng element type
            eltType = parseEltType(parser);
        } 
        else
        {
            // std type
            parser.parseType(eltType); 
        }
        
        parser.parseGreater();
        return NGTensorType::get(context, eltType, shape);
    } 
    else 
    {
        // parse nGraph scalar type
        return parseEltType(parser);
    }
}

mlir::Type NGraphOpsDialect::parseEltType(mlir::DialectAsmParser &parser) const
{
    // Process nGraph integer element types.
    MLIRContext* context = getContext();
    int width = 0;
    bool isSigned = false;
    llvm::SMLoc loc = parser.getCurrentLocation();
    if (parser.parseOptionalKeyword("i"))
    {
        parser.parseInteger(width);
        isSigned = true;
    }
    else if (parser.parseOptionalKeyword("u"))
    {
        parser.parseInteger(width);
        isSigned = true;
    }
    else if (parser.parseOptionalKeyword("bool"))
    {
        return NGBoolType::get(context);
    }
    else
    {
        parser.emitError(loc, "Invalid nGraph dialect scalar type");
        return Type();
    }
   
    switch (width)
    {
    case 8:
        return isSigned ? NGIntegerType::getInt8(context) : NGIntegerType::getUInt8(context);
    case 16:
        return isSigned ? NGIntegerType::getInt16(context) : NGIntegerType::getUInt16(context);
    case 32:
        return isSigned ? NGIntegerType::getInt32(context) : NGIntegerType::getUInt32(context);
    case 64:
        return isSigned ? NGIntegerType::getInt64(context) : NGIntegerType::getUInt64(context);
    default:
        parser.emitError(loc, "Invalid bit-width for nGraph dialect scalar type");
        return Type();
    }
}


void NGraphOpsDialect::printType(mlir::Type type, mlir::DialectAsmPrinter &printer) const
{
    switch (type.getKind())
    {
    case NG_TENSOR_TYPE_ID:
    {
        printer << "tensor<";
        auto tensorTy = type.cast<NGTensorType>();
        for (auto dim : tensorTy.getShape())
        {
            printer << dim << 'x';
        }
        printer << tensorTy.getElementType() << '>';
        return;
    }
    case NG_I8_TYPE_ID:
    case NG_I16_TYPE_ID:
    case NG_I32_TYPE_ID:
    case NG_I64_TYPE_ID:
    {
        auto intTy = type.cast<NGIntegerType>();
        printer << "i" << intTy.getWidth();
        return;
    }
    case NG_U8_TYPE_ID:
    case NG_U16_TYPE_ID:
    case NG_U32_TYPE_ID:
    case NG_U64_TYPE_ID:
    {
        auto intTy = type.cast<NGIntegerType>();
        printer << "u" << intTy.getWidth();
        return;
    }
    case NG_BOOL_TYPE_ID:
    {
        auto intTy = type.cast<NGBoolType>();
        printer << "bool";
        return;
    }
    default:
        NGRAPH_UNREACHABLE("Incorrect type to print?");
    }
    
}
