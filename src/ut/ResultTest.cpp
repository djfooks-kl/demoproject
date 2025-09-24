#include <catch2/catch_test_macros.hpp>

#include "Core/Result.h"

#pragma optimize("", off)

TEST_CASE("Simple result tests", "[xc::Result]")
{
    xc::Result<bool> resultTrue = true;
    CHECK(resultTrue.IsSuccess() == true);
    CHECK(resultTrue.IsError() == false);
    CHECK(resultTrue.Get() == true);

    xc::Result<bool> resultFalse = false;
    CHECK(resultFalse.IsSuccess() == true);
    CHECK(resultFalse.IsError() == false);
    CHECK(resultFalse.Get() == false);

    xc::Result<bool> resultError = xc::ResultError{ "Fail" };
    CHECK(resultError.IsSuccess() == false);
    CHECK(resultError.IsError() == true);
    CHECK(resultError.GetError() == "Fail");
}