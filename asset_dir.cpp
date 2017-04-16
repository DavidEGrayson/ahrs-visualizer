#include "asset_dir.h"
#include <stdlib.h>

static std::string asset_dir;

void asset_dir_init()
{
    char * env_asset_dir = getenv("ASSET_DIR");
    if (env_asset_dir)
    {
        asset_dir = env_asset_dir;
    }
    else
    {
        asset_dir = ASSET_DIR;
    }
}

std::string asset_dir_get()
{
    return asset_dir;
}
