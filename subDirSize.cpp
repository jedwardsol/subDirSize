#include "include/print.h"
#include "include/thrower.h"

#include <algorithm>

#include <filesystem>
namespace fs=std::filesystem;


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
    int64_t size{};

    std::error_code  ec{};

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





int main()
try
{
    struct SubDir
    {
        fs::path        name;
        int64_t         size;
        auto length() const {return name.string().length();}
    };

    std::vector<SubDir>     subDirs;


    for(auto subDir : fs::directory_iterator{fs::current_path()})
    {
        if(subDir.is_directory())
        {
            subDirs.emplace_back(subDir.path().string(), size(subDir));
        }
    }

    std::ranges::sort(subDirs,std::greater{}, &SubDir::size);

    auto maxLen = std::ranges::max_element(subDirs,{}, &SubDir::length)->name.string().length();


    for(auto const &subDir : subDirs)
    {
        print("{:{}} : {}\n",subDir.name.string(), maxLen, pretty(subDir.size));
    }

}
catch(std::exception const &e)
{
    print("{}",e.what());
}


