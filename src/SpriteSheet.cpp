#include <string>
#include <fstream>
#include <nlohmann/json.hpp>
#include "_Util.h"
#include "SpriteSheet.h"

namespace ng
{
SpriteSheet::SpriteSheet()
    : _pTextureManager(nullptr), _pSettings(nullptr)
{
}

void SpriteSheet::load(const std::string &name)
{
    _texture = _pTextureManager->get(name);

    _rects.clear();
    nlohmann::json json;
    std::string jsonFilename;
    jsonFilename.append(name).append(".json");
    {
        std::vector<char> buffer;
        _pSettings->readEntry(jsonFilename, buffer);

#if 0
        std::ofstream out;
        out.open(jsonFilename, std::ios::out);
        out.write(buffer.data(), buffer.size());
        out.close();
#endif

        json = nlohmann::json::parse(buffer.data());
    }

    auto jFrames = json["frames"];
    for (auto it = jFrames.begin(); it != jFrames.end(); ++it)
    {
        auto n = it.key();
        auto rect = _toRect(json["frames"][n]["frame"]);
        // std::cout << "frame " << n << " (" << rect.width << "," << rect.height << ")" << std::endl;
        _rects.insert(std::make_pair(n, rect));
        rect = _toRect(json["frames"][n]["spriteSourceSize"]);
        // std::cout << "spriteSourceSize " << n << " (" << rect.width << "," << rect.height << ")" << std::endl;
        _spriteSourceSize.insert(std::make_pair(n, rect));
        auto size = _toSize(json["frames"][n]["sourceSize"]);
        // std::cout << "sourceSize " << n << " (" << rect.width << "," << rect.height << ")" << std::endl;
        _sourceSize.insert(std::make_pair(n, size));
    }
}

bool SpriteSheet::hasRect(const std::string &name) const
{
    const auto it = _rects.find(name);
    return it != _rects.end();
}

sf::IntRect SpriteSheet::getRect(const std::string &name) const
{
    const auto it = _rects.find(name);
    return it->second;
}

sf::IntRect SpriteSheet::getSpriteSourceSize(const std::string &name) const
{
    const auto it = _spriteSourceSize.find(name);
    return it->second;
}

sf::Vector2i SpriteSheet::getSourceSize(const std::string &name) const
{
    const auto it = _sourceSize.find(name);
    return it->second;
}

} // namespace ng
