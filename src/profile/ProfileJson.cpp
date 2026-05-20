#include "ProfileJson.h"

#include <stdexcept>

namespace openref::profile {

namespace {

std::vector<dsp::MagnitudePoint> parseMagnitudePoints(const nlohmann::json& values)
{
    std::vector<dsp::MagnitudePoint> points;
    if (! values.is_array()) {
        return points;
    }
    points.reserve(values.size());
    for (const auto& value : values) {
        points.push_back({ value.value("hz", 0.0), value.value("db", 0.0) });
    }
    return points;
}

dsp::FilterType parseFilterType(const std::string& type)
{
    if (type == "lowshelf" || type == "low_shelf" || type == "LSC" || type == "LowShelf") {
        return dsp::FilterType::lowShelf;
    }
    if (type == "highshelf" || type == "high_shelf" || type == "HSC" || type == "HighShelf") {
        return dsp::FilterType::highShelf;
    }
    if (type == "lowpass" || type == "low_pass") {
        return dsp::FilterType::lowPass;
    }
    if (type == "highpass" || type == "high_pass") {
        return dsp::FilterType::highPass;
    }
    return dsp::FilterType::peaking;
}

std::string serialiseFilterType(dsp::FilterType type)
{
    switch (type) {
        case dsp::FilterType::peaking:
            return "peaking";
        case dsp::FilterType::lowShelf:
            return "lowshelf";
        case dsp::FilterType::highShelf:
            return "highshelf";
        case dsp::FilterType::lowPass:
            return "lowpass";
        case dsp::FilterType::highPass:
            return "highpass";
    }
    return "peaking";
}

std::vector<dsp::FilterSpec> parseFilters(const nlohmann::json& values)
{
    std::vector<dsp::FilterSpec> filters;
    if (! values.is_array()) {
        return filters;
    }
    filters.reserve(values.size());
    for (const auto& value : values) {
        filters.push_back({ parseFilterType(value.value("type", "peaking")),
                            value.value("freqHz", value.value("hz", 1000.0)),
                            value.value("gainDb", 0.0),
                            value.value("q", 0.70710678118) });
    }
    return filters;
}

ProfileChannel parseChannel(const nlohmann::json& json)
{
    ProfileChannel channel;
    channel.sameAs = json.value("sameAs", "");
    channel.frequencyResponse = parseMagnitudePoints(json.value("frequencyResponse", nlohmann::json::array()));
    channel.correctionCurve = parseMagnitudePoints(json.value("correctionCurve", nlohmann::json::array()));
    channel.parametricEq = parseFilters(json.value("parametricEq", nlohmann::json::array()));
    return channel;
}

} // namespace

CalibrationProfile parseCalibrationProfile(const nlohmann::json& json)
{
    if (json.value("schemaVersion", 0) != 1) {
        throw std::runtime_error("Unsupported OpenRef profile schema");
    }

    CalibrationProfile profile;
    profile.schemaVersion = 1;
    profile.id = json.value("id", "");
    profile.manufacturer = json.value("manufacturer", "");
    profile.model = json.value("model", "");
    profile.type = json.value("type", "");
    profile.recommendedPreampDb = json.value("recommendedPreampDb", 0.0);

    const auto source = json.value("source", nlohmann::json::object());
    profile.source.name = source.value("name", "");
    profile.source.url = source.value("url", "");
    profile.source.license = source.value("license", "");
    profile.source.measurementSource = source.value("measurementSource", "");
    profile.source.generatedAt = source.value("generatedAt", "");

    const auto channels = json.value("channels", nlohmann::json::object());
    profile.left = parseChannel(channels.value("left", nlohmann::json::object()));
    profile.right = parseChannel(channels.value("right", nlohmann::json::object()));
    if (profile.right.sameAs == "left") {
        profile.right = profile.left;
        profile.right.sameAs = "left";
    }

    if (profile.id.empty() || profile.left.parametricEq.empty()) {
        throw std::runtime_error("OpenRef profile is missing id or parametricEq data");
    }
    return profile;
}

TargetCurve parseTargetCurve(const nlohmann::json& json)
{
    TargetCurve curve;
    curve.id = json.value("id", "");
    curve.displayName = json.value("displayName", curve.id);
    curve.points = parseMagnitudePoints(json.value("points", nlohmann::json::array()));
    return curve;
}

SimulationProfile parseSimulationProfile(const nlohmann::json& json)
{
    SimulationProfile simulation;
    simulation.id = json.value("id", "");
    simulation.displayName = json.value("displayName", simulation.id);
    simulation.description = json.value("description", "");
    simulation.filters = parseFilters(json.value("filters", nlohmann::json::array()));
    simulation.mono = json.value("mono", false);
    return simulation;
}

nlohmann::json toJson(const CalibrationProfile& profile)
{
    auto pointsToJson = [](const std::vector<dsp::MagnitudePoint>& points) {
        auto values = nlohmann::json::array();
        for (const auto& point : points) {
            values.push_back({ { "hz", point.hz }, { "db", point.db } });
        }
        return values;
    };

    auto filtersToJson = [](const std::vector<dsp::FilterSpec>& filters) {
        auto values = nlohmann::json::array();
        for (const auto& filter : filters) {
            values.push_back({ { "type", serialiseFilterType(filter.type) },
                               { "freqHz", filter.frequencyHz },
                               { "gainDb", filter.gainDb },
                               { "q", filter.q } });
        }
        return values;
    };

    return {
        { "schemaVersion", profile.schemaVersion },
        { "id", profile.id },
        { "manufacturer", profile.manufacturer },
        { "model", profile.model },
        { "type", profile.type },
        { "source",
          { { "name", profile.source.name },
            { "url", profile.source.url },
            { "license", profile.source.license },
            { "measurementSource", profile.source.measurementSource },
            { "generatedAt", profile.source.generatedAt } } },
        { "recommendedPreampDb", profile.recommendedPreampDb },
        { "channels",
          { { "left",
              { { "frequencyResponse", pointsToJson(profile.left.frequencyResponse) },
                { "correctionCurve", pointsToJson(profile.left.correctionCurve) },
                { "parametricEq", filtersToJson(profile.left.parametricEq) } } },
            { "right", { { "sameAs", "left" } } } } }
    };
}

} // namespace openref::profile
