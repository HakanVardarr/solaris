#pragma once

#include <expected>

#define CONCAT(x, y) x##y
#define EXPECT_VOID(_T) std::expected<void, _T>
#define RETURN_ERROR(_FN)                                                   \
    if (auto CONCAT(_result_, __LINE__) = _FN; !CONCAT(_result_, __LINE__)) \
        return std::unexpected(CONCAT(_result_, __LINE__).error());
