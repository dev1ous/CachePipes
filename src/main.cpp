
#include <iostream>
#include <vector>

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

#include "dev1/algos.hpp"
#include "dev1/cache.hpp"
#include "dev1/traits.hpp"

template<>
struct dev1::traits<sf::Font> {
    static constexpr auto extensions() {
        return std::array{ ".ttf"sv, ".otf"sv, ".pfa"sv, ".pfb"sv, ".fon"sv };
    }
    static constexpr auto loader(sf::Font& font, std::filesystem::path const& filepath) {
        return font.openFromFile(filepath);
    }
};

template<>
struct dev1::traits<sf::Texture> {
    static constexpr auto extensions() {
        return std::array{ ".png"sv, ".jpg"sv, ".jpeg"sv, ".bmp"sv, ".tga"sv, ".gif"sv, ".psd"sv, ".hdr"sv, ".pic"sv, ".pnm"sv, ".pbm"sv, ".pgm"sv, ".ppm"sv };
    }
    static constexpr bool loader(sf::Texture& texture, std::string const& filepath) {
        return texture.loadFromFile(filepath);
    }
};

template<>
struct dev1::traits<sf::Music> {
    static constexpr auto extensions() {
        return std::array{ ".wav"sv, ".ogg"sv, ".flac"sv, ".mp3"sv };
    }
    static constexpr auto loader(sf::Music& music, std::string const& filepath) {
        return music.openFromFile(filepath);
    }
};

template<>
struct dev1::traits<sf::SoundBuffer> {
    static constexpr auto extensions() {
        return std::array{ ".wav"sv, ".ogg"sv, ".flac"sv };
    }
    static constexpr auto loader(sf::SoundBuffer& soundBuffer, std::filesystem::path const& filepath) {
        return soundBuffer.loadFromFile(filepath);
    }
};

int main() {
    dev1::Cache cache;

    auto resource = cache 
        | dev1::produce([] { 
            return dev1::make<sf::Font>();
        })
        | dev1::get(dev1::safety{}, "Linebeam")
        | dev1::solve_filepath("resources")
        | dev1::load_resource()
        | dev1::insert();

    auto c = resource();
    
    auto resource_2 = cache 
        | dev1::produce([] { 
            return dev1::make<sf::Font>();
        })
        | dev1::get(dev1::perf{}, "Linebeam");

    auto f = resource_2();
    auto k = f;

    std::cout << std::to_string(c.use_count()) << std::endl;
    std::cout << std::to_string(f.use_count()) << std::endl;
    std::cout << std::to_string(cache.get_size()) << std::endl;

    return 0; 
}