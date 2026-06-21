#include <print>
#include <format>
#include <algorithm>
#include <ranges>
#include <filesystem>
namespace fs=std::filesystem;

#include "include/thrower.h"
#include "include/args.h"



std::string pretty(int64_t  size)
{
    if(size > 1'000'000'000'000)
    {
        return std::format("{:.2f} Tb", size/1'000'000'000'000.0);
    }
    else if(size > 1'000'000'000)
    {
        return std::format("{:.2f} Gb", size/1'000'000'000.0);
    }
    else if(size > 1'000'000)
    {
        return std::format("{:.2f} Mb", size/1'000'000.0);
    }
    else 
    {
        return std::format("{} bytes", size);
    }
}


int64_t size(fs::path   const &path)
{
    auto size = int64_t{};
    auto ec   = std::error_code {};

    for(auto entry : fs::directory_iterator{path,ec})
    {
        if(entry.is_directory(ec))
        {
            size += ::size(entry.path());
        }
        else 
        {
            size += entry.file_size(ec);
        }
    }

    return size;
}





int main(int argc, char **argv)
try
{
    auto const args = Args{argc,argv};

    struct SubDir
    {
        fs::path        name{};
        int64_t         size{};
        auto length() const {return name.string().length();}
    };

    auto subDirs = std::vector<SubDir>{};

    for(auto subDir : fs::directory_iterator{fs::current_path()})
    {
        if(subDir.is_directory())
        {
            subDirs.emplace_back(subDir.path().string(), size(subDir));
        }
    }

    std::ranges::sort(subDirs,std::greater{}, &SubDir::size);


    if(not args.contains("-all"))
    {
        if(subDirs.size() > 10)
        {
            auto remainingCount = subDirs.size()-10;
            auto remainingSize  = std::ranges::fold_left(subDirs | std::views::drop(10) | std::views::transform(&SubDir::size), 
                                                         0ull, 
                                                         std::plus<>{});

            subDirs.resize(10);

            subDirs.emplace_back(std::format("Remaining {}...",remainingCount), remainingSize);
        }
    }

    auto maxLen = std::ranges::max_element(subDirs,{}, &SubDir::length)->name.string().length();


    for(auto const &subDir : subDirs)
    {
        std::print("{:{}} : {}\n",subDir.name.string(), maxLen, pretty(subDir.size));
    }

}
catch(std::exception const &e)
{
    std::print("{}",e.what());
}


