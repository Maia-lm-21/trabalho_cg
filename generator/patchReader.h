#ifndef PATCH_READER_H
#define PATCH_READER_H

#include <string>
#include <vector>

struct Vec3 {
    float x, y, z;
};

struct Patch {
    std::vector<int> controlPointIndices; // 16 per patch
};

// Reads the patch file and fills patches and controlPoints
bool loadPatchFile(const std::string& filename, std::vector<Patch>& patches, std::vector<Vec3>& controlPoints);

#endif 