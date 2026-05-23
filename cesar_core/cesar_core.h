#pragma once
#include "Core/Align.h"
#include "Core/Base.h"
#include "Core/Log.h"
#include "Core/Types.h"
#include "Core/Utilities.h"
#include "Core/Enum.h"
#include "Core/Hash.h"
// Note: FileWatcher is not included here to avoid pulling heavy headers into every TU.
// Include "Core/FileWatcher.h" only in sources that actually use it.
// Provide commonly used std headers here so translation units that include cesar_core.h
// still have basic STL facilities available. Prefer adding heavy headers to pch.h
// and enable project-level PCH to avoid reparsing these for every TU.
#include <set>
#include <array>
#include <cmath>
#include <vector>
#include <memory>
#include <string>
#include <iomanip>
#include <sstream>
#include <algorithm>
#include <functional>
#include <filesystem>
#include <unordered_set>
#include <unordered_map>

#include <Tracy/tracy/Tracy.hpp>


