#pragma once

#include "CalibrationProfile.h"

#include <filesystem>
#include <vector>

namespace openref::profile {

class ProfileManager {
public:
    static std::filesystem::path userProfileDirectory();
    static std::filesystem::path userPresetDirectory();
    std::vector<CalibrationProfile> loadProfilesFromDirectory(const std::filesystem::path& directory) const;
};

} // namespace openref::profile
