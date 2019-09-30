#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <mutex>
#include <ustd/log.h>
#include <sys/wait.h>
#include <iostream>
#include <string.h>
#include <unistd.h>

#include "oss_handler.h"
int main()
{
    using namespace algorithmservice::business;
    using namespace AlibabaCloud::OSS;
#define UPLOAD

#ifdef UPLOAD
    OssHandle oss_handle_;
    OssInfo oss = {"LTAIk5l3xtfcuZWN", "tLhxZTvhknHBjc62FqT2QONoj15wfG", "https://oss-cn-hangzhou.aliyuncs.com"};
    ObjectInfo obj = {"t-3datahz", "dental/3mf_test_3", "to_upload", "dentalbeb.zip"};
    bool ret = oss_handle_.upload(oss, obj);
    printf("ret: %d\n", ret);
#endif

#ifdef DOWNLOAD
    OssHandle oss_handle_;

    OssInfo oss = {"LTAIk5l3xtfcuZWN", "tLhxZTvhknHBjc62FqT2QONoj15wfG", "https://oss-cn-hangzhou.aliyuncs.com"};
    ObjectInfo obj = {"t-3datahz", "dental/test", "download", ""};
    bool ret = oss_handle_.download(oss, obj);
    printf("ret: %d\n", ret);
#endif
    return 0;
}