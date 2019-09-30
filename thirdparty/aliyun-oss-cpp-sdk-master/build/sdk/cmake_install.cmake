# Install script for directory: /home/algorithm/thirdparty/aliyun-oss-cpp-sdk-master/sdk

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/usr/local")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Install shared libraries without execute permission?
if(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)
  set(CMAKE_INSTALL_SO_NO_EXE "0")
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/alibabacloud/oss/auth" TYPE FILE FILES
    "/home/algorithm/thirdparty/aliyun-oss-cpp-sdk-master/sdk/include/alibabacloud/oss/auth/Credentials.h"
    "/home/algorithm/thirdparty/aliyun-oss-cpp-sdk-master/sdk/include/alibabacloud/oss/auth/CredentialsProvider.h"
    )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/alibabacloud/oss/client" TYPE FILE FILES
    "/home/algorithm/thirdparty/aliyun-oss-cpp-sdk-master/sdk/include/alibabacloud/oss/client/AsyncCallerContext.h"
    "/home/algorithm/thirdparty/aliyun-oss-cpp-sdk-master/sdk/include/alibabacloud/oss/client/ClientConfiguration.h"
    "/home/algorithm/thirdparty/aliyun-oss-cpp-sdk-master/sdk/include/alibabacloud/oss/client/Error.h"
    "/home/algorithm/thirdparty/aliyun-oss-cpp-sdk-master/sdk/include/alibabacloud/oss/client/RateLimiter.h"
    "/home/algorithm/thirdparty/aliyun-oss-cpp-sdk-master/sdk/include/alibabacloud/oss/client/RetryStrategy.h"
    )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/alibabacloud/oss/http" TYPE FILE FILES "/home/algorithm/thirdparty/aliyun-oss-cpp-sdk-master/sdk/include/alibabacloud/oss/http/HttpType.h")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/alibabacloud/oss/utils" TYPE FILE FILES
    "/home/algorithm/thirdparty/aliyun-oss-cpp-sdk-master/sdk/include/alibabacloud/oss/utils/Outcome.h"
    "/home/algorithm/thirdparty/aliyun-oss-cpp-sdk-master/sdk/include/alibabacloud/oss/utils/Runnable.h"
    )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/alibabacloud/oss/model" TYPE FILE FILES
    "/home/algorithm/thirdparty/aliyun-oss-cpp-sdk-master/sdk/include/alibabacloud/oss/model/AbortMultipartUploadRequest.h"
    "/home/algorithm/thirdparty/aliyun-oss-cpp-sdk-master/sdk/include/alibabacloud/oss/model/AppendObjectRequest.h"
    "/home/algorithm/thirdparty/aliyun-oss-cpp-sdk-master/sdk/include/alibabacloud/oss/model/AppendObjectResult.h"
    "/home/algorithm/thirdparty/aliyun-oss-cpp-sdk-master/sdk/include/alibabacloud/oss/model/Bucket.h"
    "/home/algorithm/thirdparty/aliyun-oss-cpp-sdk-master/sdk/include/alibabacloud/oss/model/CORSRule.h"
    "/home/algorithm/thirdparty/aliyun-oss-cpp-sdk-master/sdk/include/alibabacloud/oss/model/CompleteMultipartUploadRequest.h"
    "/home/algorithm/thirdparty/aliyun-oss-cpp-sdk-master/sdk/include/alibabacloud/oss/model/CompleteMultipartUploadResult.h"
    "/home/algorithm/thirdparty/aliyun-oss-cpp-sdk-master/sdk/include/alibabacloud/oss/model/CopyObjectRequest.h"
    "/home/algorithm/thirdparty/aliyun-oss-cpp-sdk-master/sdk/include/alibabacloud/oss/model/CopyObjectResult.h"
    "/home/algorithm/thirdparty/aliyun-oss-cpp-sdk-master/sdk/include/alibabacloud/oss/model/CreateBucketRequest.h"
    "/home/algorithm/thirdparty/aliyun-oss-cpp-sdk-master/sdk/include/alibabacloud/oss/model/CreateSelectObjectMetaRequest.h"
    "/home/algorithm/thirdparty/aliyun-oss-cpp-sdk-master/sdk/include/alibabacloud/oss/model/CreateSelectObjectMetaResult.h"
    "/home/algorithm/thirdparty/aliyun-oss-cpp-sdk-master/sdk/include/alibabacloud/oss/model/CreateSymlinkRequest.h"
    "/home/algorithm/thirdparty/aliyun-oss-cpp-sdk-master/sdk/include/alibabacloud/oss/model/CreateSymlinkResult.h"
    "/home/algorithm/thirdparty/aliyun-oss-cpp-sdk-master/sdk/include/alibabacloud/oss/model/DeleteBucketCorsRequest.h"
    "/home/algorithm/thirdparty/aliyun-oss-cpp-sdk-master/sdk/include/alibabacloud/oss/model/DeleteBucketLifecycleRequest.h"
    "/home/algorithm/thirdparty/aliyun-oss-cpp-sdk-master/sdk/include/alibabacloud/oss/model/DeleteBucketLoggingRequest.h"
    "/home/algorithm/thirdparty/aliyun-oss-cpp-sdk-master/sdk/include/alibabacloud/oss/model/DeleteBucketPolicyRequest.h"
    "/home/algorithm/thirdparty/aliyun-oss-cpp-sdk-master/sdk/include/alibabacloud/oss/model/DeleteBucketRequest.h"
    "/home/algorithm/thirdparty/aliyun-oss-cpp-sdk-master/sdk/include/alibabacloud/oss/model/DeleteBucketWebsiteRequest.h"
    "/home/algorithm/thirdparty/aliyun-oss-cpp-sdk-master/sdk/include/alibabacloud/oss/model/DeleteLiveChannelRequest.h"
    "/home/algorithm/thirdparty/aliyun-oss-cpp-sdk-master/sdk/include/alibabacloud/oss/model/DeleteObjectRequest.h"
    "/home/algorithm/thirdparty/aliyun-oss-cpp-sdk-master/sdk/include/alibabacloud/oss/model/DeleteObjectTaggingRequest.h"
    "/home/algorithm/thirdparty/aliyun-oss-cpp-sdk-master/sdk/include/alibabacloud/oss/model/DeleteObjectTaggingResult.h"
    "/home/algorithm/thirdparty/aliyun-oss-cpp-sdk-master/sdk/include/alibabacloud/oss/model/DeleteObjectsRequest.h"
    "/home/algorithm/thirdparty/aliyun-oss-cpp-sdk-master/sdk/include/alibabacloud/oss/model/DeleteObjectsResult.h"
    "/home/algorithm/thirdparty/aliyun-oss-cpp-sdk-master/sdk/include/alibabacloud/oss/model/DownloadObjectRequest.h"
    "/home/algorithm/thirdparty/aliyun-oss-cpp-sdk-master/sdk/include/alibabacloud/oss/model/GeneratePresignedUrlRequest.h"
    "/home/algorithm/thirdparty/aliyun-oss-cpp-sdk-master/sdk/include/alibabacloud/oss/model/GenerateRTMPSignedUrlRequest.h"
    "/home/algorithm/thirdparty/aliyun-oss-cpp-sdk-master/sdk/include/alibabacloud/oss/model/GetBucketAclRequest.h"
    "/home/algorithm/thirdparty/aliyun-oss-cpp-sdk-master/sdk/include/alibabacloud/oss/model/GetBucketAclResult.h"
    "/home/algorithm/thirdparty/aliyun-oss-cpp-sdk-master/sdk/include/alibabacloud/oss/model/GetBucketCorsRequest.h"
    "/home/algorithm/thirdparty/aliyun-oss-cpp-sdk-master/sdk/include/alibabacloud/oss/model/GetBucketCorsResult.h"
    "/home/algorithm/thirdparty/aliyun-oss-cpp-sdk-master/sdk/include/alibabacloud/oss/model/GetBucketInfoRequest.h"
    "/home/algorithm/thirdparty/aliyun-oss-cpp-sdk-master/sdk/include/alibabacloud/oss/model/GetBucketInfoResult.h"
    "/home/algorithm/thirdparty/aliyun-oss-cpp-sdk-master/sdk/include/alibabacloud/oss/model/GetBucketLifecycleRequest.h"
    "/home/algorithm/thirdparty/aliyun-oss-cpp-sdk-master/sdk/include/alibabacloud/oss/model/GetBucketLifecycleResult.h"
    "/home/algorithm/thirdparty/aliyun-oss-cpp-sdk-master/sdk/include/alibabacloud/oss/model/GetBucketLocationRequest.h"
    "/home/algorithm/thirdparty/aliyun-oss-cpp-sdk-master/sdk/include/alibabacloud/oss/model/GetBucketLocationResult.h"
    "/home/algorithm/thirdparty/aliyun-oss-cpp-sdk-master/sdk/include/alibabacloud/oss/model/GetBucketLoggingRequest.h"
    "/home/algorithm/thirdparty/aliyun-oss-cpp-sdk-master/sdk/include/alibabacloud/oss/model/GetBucketLoggingResult.h"
    "/home/algorithm/thirdparty/aliyun-oss-cpp-sdk-master/sdk/include/alibabacloud/oss/model/GetBucketPaymentRequest.h"
    "/home/algorithm/thirdparty/aliyun-oss-cpp-sdk-master/sdk/include/alibabacloud/oss/model/GetBucketPaymentResult.h"
    "/home/algorithm/thirdparty/aliyun-oss-cpp-sdk-master/sdk/include/alibabacloud/oss/model/GetBucketPolicyRequest.h"
    "/home/algorithm/thirdparty/aliyun-oss-cpp-sdk-master/sdk/include/alibabacloud/oss/model/GetBucketPolicyResult.h"
    "/home/algorithm/thirdparty/aliyun-oss-cpp-sdk-master/sdk/include/alibabacloud/oss/model/GetBucketRefererRequest.h"
    "/home/algorithm/thirdparty/aliyun-oss-cpp-sdk-master/sdk/include/alibabacloud/oss/model/GetBucketRefererResult.h"
    "/home/algorithm/thirdparty/aliyun-oss-cpp-sdk-master/sdk/include/alibabacloud/oss/model/GetBucketStatRequest.h"
    "/home/algorithm/thirdparty/aliyun-oss-cpp-sdk-master/sdk/include/alibabacloud/oss/model/GetBucketStatResult.h"
    "/home/algorithm/thirdparty/aliyun-oss-cpp-sdk-master/sdk/include/alibabacloud/oss/model/GetBucketStorageCapacityRequest.h"
    "/home/algorithm/thirdparty/aliyun-oss-cpp-sdk-master/sdk/include/alibabacloud/oss/model/GetBucketStorageCapacityResult.h"
    "/home/algorithm/thirdparty/aliyun-oss-cpp-sdk-master/sdk/include/alibabacloud/oss/model/GetBucketWebsiteRequest.h"
    "/home/algorithm/thirdparty/aliyun-oss-cpp-sdk-master/sdk/include/alibabacloud/oss/model/GetBucketWebsiteResult.h"
    "/home/algorithm/thirdparty/aliyun-oss-cpp-sdk-master/sdk/include/alibabacloud/oss/model/GetLiveChannelHistoryRequest.h"
    "/home/algorithm/thirdparty/aliyun-oss-cpp-sdk-master/sdk/include/alibabacloud/oss/model/GetLiveChannelHistoryResult.h"
    "/home/algorithm/thirdparty/aliyun-oss-cpp-sdk-master/sdk/include/alibabacloud/oss/model/GetLiveChannelInfoRequest.h"
    "/home/algorithm/thirdparty/aliyun-oss-cpp-sdk-master/sdk/include/alibabacloud/oss/model/GetLiveChannelInfoResult.h"
    "/home/algorithm/thirdparty/aliyun-oss-cpp-sdk-master/sdk/include/alibabacloud/oss/model/GetLiveChannelStatRequest.h"
    "/home/algorithm/thirdparty/aliyun-oss-cpp-sdk-master/sdk/include/alibabacloud/oss/model/GetLiveChannelStatResult.h"
    "/home/algorithm/thirdparty/aliyun-oss-cpp-sdk-master/sdk/include/alibabacloud/oss/model/GetObjectAclRequest.h"
    "/home/algorithm/thirdparty/aliyun-oss-cpp-sdk-master/sdk/include/alibabacloud/oss/model/GetObjectAclResult.h"
    "/home/algorithm/thirdparty/aliyun-oss-cpp-sdk-master/sdk/include/alibabacloud/oss/model/GetObjectByUrlRequest.h"
    "/home/algorithm/thirdparty/aliyun-oss-cpp-sdk-master/sdk/include/alibabacloud/oss/model/GetObjectMetaRequest.h"
    "/home/algorithm/thirdparty/aliyun-oss-cpp-sdk-master/sdk/include/alibabacloud/oss/model/GetObjectRequest.h"
    "/home/algorithm/thirdparty/aliyun-oss-cpp-sdk-master/sdk/include/alibabacloud/oss/model/GetObjectResult.h"
    "/home/algorithm/thirdparty/aliyun-oss-cpp-sdk-master/sdk/include/alibabacloud/oss/model/GetObjectTaggingRequest.h"
    "/home/algorithm/thirdparty/aliyun-oss-cpp-sdk-master/sdk/include/alibabacloud/oss/model/GetObjectTaggingResult.h"
    "/home/algorithm/thirdparty/aliyun-oss-cpp-sdk-master/sdk/include/alibabacloud/oss/model/GetSymlinkRequest.h"
    "/home/algorithm/thirdparty/aliyun-oss-cpp-sdk-master/sdk/include/alibabacloud/oss/model/GetSymlinkResult.h"
    "/home/algorithm/thirdparty/aliyun-oss-cpp-sdk-master/sdk/include/alibabacloud/oss/model/GetVodPlaylistRequest.h"
    "/home/algorithm/thirdparty/aliyun-oss-cpp-sdk-master/sdk/include/alibabacloud/oss/model/GetVodPlaylistResult.h"
    "/home/algorithm/thirdparty/aliyun-oss-cpp-sdk-master/sdk/include/alibabacloud/oss/model/HeadObjectRequest.h"
    "/home/algorithm/thirdparty/aliyun-oss-cpp-sdk-master/sdk/include/alibabacloud/oss/model/InitiateMultipartUploadRequest.h"
    "/home/algorithm/thirdparty/aliyun-oss-cpp-sdk-master/sdk/include/alibabacloud/oss/model/InitiateMultipartUploadResult.h"
    "/home/algorithm/thirdparty/aliyun-oss-cpp-sdk-master/sdk/include/alibabacloud/oss/model/InputFormat.h"
    "/home/algorithm/thirdparty/aliyun-oss-cpp-sdk-master/sdk/include/alibabacloud/oss/model/LifecycleRule.h"
    "/home/algorithm/thirdparty/aliyun-oss-cpp-sdk-master/sdk/include/alibabacloud/oss/model/ListBucketsRequest.h"
    "/home/algorithm/thirdparty/aliyun-oss-cpp-sdk-master/sdk/include/alibabacloud/oss/model/ListBucketsResult.h"
    "/home/algorithm/thirdparty/aliyun-oss-cpp-sdk-master/sdk/include/alibabacloud/oss/model/ListLiveChannelRequest.h"
    "/home/algorithm/thirdparty/aliyun-oss-cpp-sdk-master/sdk/include/alibabacloud/oss/model/ListLiveChannelResult.h"
    "/home/algorithm/thirdparty/aliyun-oss-cpp-sdk-master/sdk/include/alibabacloud/oss/model/ListMultipartUploadsRequest.h"
    "/home/algorithm/thirdparty/aliyun-oss-cpp-sdk-master/sdk/include/alibabacloud/oss/model/ListMultipartUploadsResult.h"
    "/home/algorithm/thirdparty/aliyun-oss-cpp-sdk-master/sdk/include/alibabacloud/oss/model/ListObjectsRequest.h"
    "/home/algorithm/thirdparty/aliyun-oss-cpp-sdk-master/sdk/include/alibabacloud/oss/model/ListObjectsResult.h"
    "/home/algorithm/thirdparty/aliyun-oss-cpp-sdk-master/sdk/include/alibabacloud/oss/model/ListPartsRequest.h"
    "/home/algorithm/thirdparty/aliyun-oss-cpp-sdk-master/sdk/include/alibabacloud/oss/model/ListPartsResult.h"
    "/home/algorithm/thirdparty/aliyun-oss-cpp-sdk-master/sdk/include/alibabacloud/oss/model/MultiCopyObjectRequest.h"
    "/home/algorithm/thirdparty/aliyun-oss-cpp-sdk-master/sdk/include/alibabacloud/oss/model/ObjectCallbackBuilder.h"
    "/home/algorithm/thirdparty/aliyun-oss-cpp-sdk-master/sdk/include/alibabacloud/oss/model/ObjectMetaData.h"
    "/home/algorithm/thirdparty/aliyun-oss-cpp-sdk-master/sdk/include/alibabacloud/oss/model/OutputFormat.h"
    "/home/algorithm/thirdparty/aliyun-oss-cpp-sdk-master/sdk/include/alibabacloud/oss/model/Owner.h"
    "/home/algorithm/thirdparty/aliyun-oss-cpp-sdk-master/sdk/include/alibabacloud/oss/model/Part.h"
    "/home/algorithm/thirdparty/aliyun-oss-cpp-sdk-master/sdk/include/alibabacloud/oss/model/PostVodPlaylistRequest.h"
    "/home/algorithm/thirdparty/aliyun-oss-cpp-sdk-master/sdk/include/alibabacloud/oss/model/ProcessObjectRequest.h"
    "/home/algorithm/thirdparty/aliyun-oss-cpp-sdk-master/sdk/include/alibabacloud/oss/model/PutLiveChannelRequest.h"
    "/home/algorithm/thirdparty/aliyun-oss-cpp-sdk-master/sdk/include/alibabacloud/oss/model/PutLiveChannelResult.h"
    "/home/algorithm/thirdparty/aliyun-oss-cpp-sdk-master/sdk/include/alibabacloud/oss/model/PutLiveChannelStatusRequest.h"
    "/home/algorithm/thirdparty/aliyun-oss-cpp-sdk-master/sdk/include/alibabacloud/oss/model/PutObjectByUrlRequest.h"
    "/home/algorithm/thirdparty/aliyun-oss-cpp-sdk-master/sdk/include/alibabacloud/oss/model/PutObjectRequest.h"
    "/home/algorithm/thirdparty/aliyun-oss-cpp-sdk-master/sdk/include/alibabacloud/oss/model/PutObjectResult.h"
    "/home/algorithm/thirdparty/aliyun-oss-cpp-sdk-master/sdk/include/alibabacloud/oss/model/RestoreObjectRequest.h"
    "/home/algorithm/thirdparty/aliyun-oss-cpp-sdk-master/sdk/include/alibabacloud/oss/model/SelectObjectRequest.h"
    "/home/algorithm/thirdparty/aliyun-oss-cpp-sdk-master/sdk/include/alibabacloud/oss/model/SetBucketAclRequest.h"
    "/home/algorithm/thirdparty/aliyun-oss-cpp-sdk-master/sdk/include/alibabacloud/oss/model/SetBucketCorsRequest.h"
    "/home/algorithm/thirdparty/aliyun-oss-cpp-sdk-master/sdk/include/alibabacloud/oss/model/SetBucketLifecycleRequest.h"
    "/home/algorithm/thirdparty/aliyun-oss-cpp-sdk-master/sdk/include/alibabacloud/oss/model/SetBucketLoggingRequest.h"
    "/home/algorithm/thirdparty/aliyun-oss-cpp-sdk-master/sdk/include/alibabacloud/oss/model/SetBucketPaymentRequest.h"
    "/home/algorithm/thirdparty/aliyun-oss-cpp-sdk-master/sdk/include/alibabacloud/oss/model/SetBucketPolicyRequest.h"
    "/home/algorithm/thirdparty/aliyun-oss-cpp-sdk-master/sdk/include/alibabacloud/oss/model/SetBucketRefererRequest.h"
    "/home/algorithm/thirdparty/aliyun-oss-cpp-sdk-master/sdk/include/alibabacloud/oss/model/SetBucketStorageCapacityRequest.h"
    "/home/algorithm/thirdparty/aliyun-oss-cpp-sdk-master/sdk/include/alibabacloud/oss/model/SetBucketWebsiteRequest.h"
    "/home/algorithm/thirdparty/aliyun-oss-cpp-sdk-master/sdk/include/alibabacloud/oss/model/SetObjectAclRequest.h"
    "/home/algorithm/thirdparty/aliyun-oss-cpp-sdk-master/sdk/include/alibabacloud/oss/model/SetObjectTaggingRequest.h"
    "/home/algorithm/thirdparty/aliyun-oss-cpp-sdk-master/sdk/include/alibabacloud/oss/model/SetObjectTaggingResult.h"
    "/home/algorithm/thirdparty/aliyun-oss-cpp-sdk-master/sdk/include/alibabacloud/oss/model/Tagging.h"
    "/home/algorithm/thirdparty/aliyun-oss-cpp-sdk-master/sdk/include/alibabacloud/oss/model/UploadObjectRequest.h"
    "/home/algorithm/thirdparty/aliyun-oss-cpp-sdk-master/sdk/include/alibabacloud/oss/model/UploadPartCopyRequest.h"
    "/home/algorithm/thirdparty/aliyun-oss-cpp-sdk-master/sdk/include/alibabacloud/oss/model/UploadPartCopyResult.h"
    "/home/algorithm/thirdparty/aliyun-oss-cpp-sdk-master/sdk/include/alibabacloud/oss/model/UploadPartRequest.h"
    "/home/algorithm/thirdparty/aliyun-oss-cpp-sdk-master/sdk/include/alibabacloud/oss/model/VoidResult.h"
    )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/alibabacloud/oss" TYPE FILE FILES
    "/home/algorithm/thirdparty/aliyun-oss-cpp-sdk-master/sdk/include/alibabacloud/oss/Config.h"
    "/home/algorithm/thirdparty/aliyun-oss-cpp-sdk-master/sdk/include/alibabacloud/oss/Const.h"
    "/home/algorithm/thirdparty/aliyun-oss-cpp-sdk-master/sdk/include/alibabacloud/oss/Export.h"
    "/home/algorithm/thirdparty/aliyun-oss-cpp-sdk-master/sdk/include/alibabacloud/oss/Global.h"
    "/home/algorithm/thirdparty/aliyun-oss-cpp-sdk-master/sdk/include/alibabacloud/oss/OssClient.h"
    "/home/algorithm/thirdparty/aliyun-oss-cpp-sdk-master/sdk/include/alibabacloud/oss/OssError.h"
    "/home/algorithm/thirdparty/aliyun-oss-cpp-sdk-master/sdk/include/alibabacloud/oss/OssFwd.h"
    "/home/algorithm/thirdparty/aliyun-oss-cpp-sdk-master/sdk/include/alibabacloud/oss/OssRequest.h"
    "/home/algorithm/thirdparty/aliyun-oss-cpp-sdk-master/sdk/include/alibabacloud/oss/OssResponse.h"
    "/home/algorithm/thirdparty/aliyun-oss-cpp-sdk-master/sdk/include/alibabacloud/oss/OssResult.h"
    "/home/algorithm/thirdparty/aliyun-oss-cpp-sdk-master/sdk/include/alibabacloud/oss/ServiceRequest.h"
    "/home/algorithm/thirdparty/aliyun-oss-cpp-sdk-master/sdk/include/alibabacloud/oss/ServiceResult.h"
    "/home/algorithm/thirdparty/aliyun-oss-cpp-sdk-master/sdk/include/alibabacloud/oss/Types.h"
    )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib64" TYPE STATIC_LIBRARY FILES "/home/algorithm/thirdparty/aliyun-oss-cpp-sdk-master/build/lib/libalibabacloud-oss-cpp-sdk.a")
endif()

