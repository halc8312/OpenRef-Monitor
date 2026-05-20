#include "profile/ProfileJson.h"

#include <catch2/catch_test_macros.hpp>

using namespace openref::profile;

TEST_CASE("Calibration profile parser accepts schema v1")
{
    const auto json = nlohmann::json::parse(R"json(
    {
      "schemaVersion": 1,
      "id": "test",
      "manufacturer": "Audio-Technica",
      "model": "ATH-M70x",
      "type": "headphone",
      "source": {
        "name": "AutoEq",
        "url": "https://github.com/jaakkopasanen/AutoEq",
        "license": "MIT",
        "measurementSource": "test"
      },
      "recommendedPreampDb": -6.0,
      "channels": {
        "left": {
          "frequencyResponse": [{ "hz": 20, "db": 1 }],
          "correctionCurve": [{ "hz": 20, "db": -1 }],
          "parametricEq": [{ "type": "peaking", "freqHz": 1000, "gainDb": 1, "q": 1 }]
        },
        "right": { "sameAs": "left" }
      }
    }
    )json");

    const auto profile = parseCalibrationProfile(json);
    REQUIRE(profile.id == "test");
    REQUIRE(profile.left.parametricEq.size() == 1);
    REQUIRE(profile.right.parametricEq.size() == 1);
}
