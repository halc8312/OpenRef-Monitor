#include "ProfileManager.h"

#include "ProfileJson.h"

#include <cstdlib>
#include <fstream>

namespace openref::profile {

std::filesystem::path ProfileManager::userProfileDirectory()
{
#if defined(_WIN32)
    const auto* appData = std::getenv("APPDATA");
    const std::filesystem::path base = appData == nullptr ? std::filesystem::path {} : appData;
    return base / "OpenRefMonitor" / "profiles";
#else
    const auto* home = std::getenv("HOME");
    const std::filesystem::path base = home == nullptr ? std::filesystem::path {} : home;
    return base / ".config" / "OpenRefMonitor" / "profiles";
#endif
}

std::filesystem::path ProfileManager::userPresetDirectory()
{
#if defined(_WIN32)
    const auto* appData = std::getenv("APPDATA");
    const std::filesystem::path base = appData == nullptr ? std::filesystem::path {} : appData;
    return base / "OpenRefMonitor" / "presets";
#else
    const auto* home = std::getenv("HOME");
    const std::filesystem::path base = home == nullptr ? std::filesystem::path {} : home;
    return base / ".config" / "OpenRefMonitor" / "presets";
#endif
}

std::vector<CalibrationProfile> ProfileManager::loadProfilesFromDirectory(const std::filesystem::path& directory) const
{
    std::vector<CalibrationProfile> profiles;
    if (! std::filesystem::exists(directory)) {
        return profiles;
    }

    for (const auto& entry : std::filesystem::directory_iterator(directory)) {
        if (! entry.is_regular_file() || entry.path().extension() != ".json") {
            continue;
        }

        std::ifstream input(entry.path());
        if (! input) {
            continue;
        }

        try {
            profiles.push_back(parseCalibrationProfile(nlohmann::json::parse(input)));
        } catch (...) {
        }
    }
    return profiles;
}

} // namespace openref::profile
