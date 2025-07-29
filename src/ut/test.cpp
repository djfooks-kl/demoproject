#include <catch2/catch_test_macros.hpp>

#include <flecs/flecs.h>
#include "Demo.h"

TEST_CASE("Example test", "[example]")
{
    flecs::world world;
    world.entity();

    REQUIRE(1 + 1 == 2);
}