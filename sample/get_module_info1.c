/* -*- Mode: c; tab-width: 4; c-basic-offset: 4; indent-tabs-mode: nil; -*- */
#include <stdio.h>
#include "concurrent/concurrent.h"

int main(void)
{
    int VersionMajor;
    int VersionMinor;
    int VersionRevision;
    int HasLongLong;
    int IsDebugBuild;
    int IsStackGrowthDownward;
    unsigned long MinimumStackSize;
    const char *LicenseName;
    const char *ProjectURL;
    const char *BuildDateTime;
    const char *BuildInfo;
    const char *ArchName;
    Concurrent_GetModuleInfo(&VersionMajor,
                             &VersionMinor,
                             &VersionRevision,
                             &HasLongLong,
                             &IsDebugBuild,
                             &IsStackGrowthDownward,
                             &MinimumStackSize,
                             &LicenseName,
                             &ProjectURL,
                             &BuildDateTime,
                             &BuildInfo,
                             &ArchName);
    printf("libconcurrent module information:\n");
    printf("- version: %d.%d.%d\n", VersionMajor, VersionMinor, VersionRevision);
    printf("- has long long: %s\n", HasLongLong ? "YES" : "NO");
    printf("- is debug build: %s\n", IsDebugBuild ? "YES" : "NO");
    printf("- is stack growth downward: %s\n", IsStackGrowthDownward ? "YES" : "NO");
    printf("- minimum stack size: %lu bytes\n", MinimumStackSize);
    printf("- license: %s\n", LicenseName);
    printf("- project URL: %s\n", ProjectURL);
    printf("- build date: %s\n", BuildDateTime);
    printf("- build info: %s\n", BuildInfo);
    printf("- arch name: %s\n", ArchName);
    return 0;
}
