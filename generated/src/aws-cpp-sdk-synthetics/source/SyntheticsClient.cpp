﻿/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#include <aws/core/utils/Outcome.h>
#include <aws/core/auth/AWSAuthSigner.h>
#include <aws/core/client/CoreErrors.h>
#include <aws/core/client/RetryStrategy.h>
#include <aws/core/http/HttpClient.h>
#include <aws/core/http/HttpResponse.h>
#include <aws/core/http/HttpClientFactory.h>
#include <aws/core/auth/AWSCredentialsProviderChain.h>
#include <aws/core/utils/json/JsonSerializer.h>
#include <aws/core/utils/memory/stl/AWSStringStream.h>
#include <aws/core/utils/threading/Executor.h>
#include <aws/core/utils/DNS.h>
#include <aws/core/utils/logging/LogMacros.h>
#include <aws/core/utils/logging/ErrorMacros.h>

#include <aws/synthetics/SyntheticsClient.h>
#include <aws/synthetics/SyntheticsErrorMarshaller.h>
#include <aws/synthetics/SyntheticsEndpointProvider.h>
#include <aws/synthetics/model/AssociateResourceRequest.h>
#include <aws/synthetics/model/CreateCanaryRequest.h>
#include <aws/synthetics/model/CreateGroupRequest.h>
#include <aws/synthetics/model/DeleteCanaryRequest.h>
#include <aws/synthetics/model/DeleteGroupRequest.h>
#include <aws/synthetics/model/DescribeCanariesRequest.h>
#include <aws/synthetics/model/DescribeCanariesLastRunRequest.h>
#include <aws/synthetics/model/DescribeRuntimeVersionsRequest.h>
#include <aws/synthetics/model/DisassociateResourceRequest.h>
#include <aws/synthetics/model/GetCanaryRequest.h>
#include <aws/synthetics/model/GetCanaryRunsRequest.h>
#include <aws/synthetics/model/GetGroupRequest.h>
#include <aws/synthetics/model/ListAssociatedGroupsRequest.h>
#include <aws/synthetics/model/ListGroupResourcesRequest.h>
#include <aws/synthetics/model/ListGroupsRequest.h>
#include <aws/synthetics/model/ListTagsForResourceRequest.h>
#include <aws/synthetics/model/StartCanaryRequest.h>
#include <aws/synthetics/model/StopCanaryRequest.h>
#include <aws/synthetics/model/TagResourceRequest.h>
#include <aws/synthetics/model/UntagResourceRequest.h>
#include <aws/synthetics/model/UpdateCanaryRequest.h>

using namespace Aws;
using namespace Aws::Auth;
using namespace Aws::Client;
using namespace Aws::Synthetics;
using namespace Aws::Synthetics::Model;
using namespace Aws::Http;
using namespace Aws::Utils::Json;
using ResolveEndpointOutcome = Aws::Endpoint::ResolveEndpointOutcome;

const char* SyntheticsClient::SERVICE_NAME = "synthetics";
const char* SyntheticsClient::ALLOCATION_TAG = "SyntheticsClient";

SyntheticsClient::SyntheticsClient(const Synthetics::SyntheticsClientConfiguration& clientConfiguration,
                                   std::shared_ptr<SyntheticsEndpointProviderBase> endpointProvider) :
  BASECLASS(clientConfiguration,
            Aws::MakeShared<AWSAuthV4Signer>(ALLOCATION_TAG,
                                             Aws::MakeShared<DefaultAWSCredentialsProviderChain>(ALLOCATION_TAG),
                                             SERVICE_NAME,
                                             Aws::Region::ComputeSignerRegion(clientConfiguration.region)),
            Aws::MakeShared<SyntheticsErrorMarshaller>(ALLOCATION_TAG)),
  m_clientConfiguration(clientConfiguration),
  m_executor(clientConfiguration.executor),
  m_endpointProvider(std::move(endpointProvider))
{
  init(m_clientConfiguration);
}

SyntheticsClient::SyntheticsClient(const AWSCredentials& credentials,
                                   std::shared_ptr<SyntheticsEndpointProviderBase> endpointProvider,
                                   const Synthetics::SyntheticsClientConfiguration& clientConfiguration) :
  BASECLASS(clientConfiguration,
            Aws::MakeShared<AWSAuthV4Signer>(ALLOCATION_TAG,
                                             Aws::MakeShared<SimpleAWSCredentialsProvider>(ALLOCATION_TAG, credentials),
                                             SERVICE_NAME,
                                             Aws::Region::ComputeSignerRegion(clientConfiguration.region)),
            Aws::MakeShared<SyntheticsErrorMarshaller>(ALLOCATION_TAG)),
    m_clientConfiguration(clientConfiguration),
    m_executor(clientConfiguration.executor),
    m_endpointProvider(std::move(endpointProvider))
{
  init(m_clientConfiguration);
}

SyntheticsClient::SyntheticsClient(const std::shared_ptr<AWSCredentialsProvider>& credentialsProvider,
                                   std::shared_ptr<SyntheticsEndpointProviderBase> endpointProvider,
                                   const Synthetics::SyntheticsClientConfiguration& clientConfiguration) :
  BASECLASS(clientConfiguration,
            Aws::MakeShared<AWSAuthV4Signer>(ALLOCATION_TAG,
                                             credentialsProvider,
                                             SERVICE_NAME,
                                             Aws::Region::ComputeSignerRegion(clientConfiguration.region)),
            Aws::MakeShared<SyntheticsErrorMarshaller>(ALLOCATION_TAG)),
    m_clientConfiguration(clientConfiguration),
    m_executor(clientConfiguration.executor),
    m_endpointProvider(std::move(endpointProvider))
{
  init(m_clientConfiguration);
}

    /* Legacy constructors due deprecation */
  SyntheticsClient::SyntheticsClient(const Client::ClientConfiguration& clientConfiguration) :
  BASECLASS(clientConfiguration,
            Aws::MakeShared<AWSAuthV4Signer>(ALLOCATION_TAG,
                                             Aws::MakeShared<DefaultAWSCredentialsProviderChain>(ALLOCATION_TAG),
                                             SERVICE_NAME,
                                             Aws::Region::ComputeSignerRegion(clientConfiguration.region)),
            Aws::MakeShared<SyntheticsErrorMarshaller>(ALLOCATION_TAG)),
  m_clientConfiguration(clientConfiguration),
  m_executor(clientConfiguration.executor),
  m_endpointProvider(Aws::MakeShared<SyntheticsEndpointProvider>(ALLOCATION_TAG))
{
  init(m_clientConfiguration);
}

SyntheticsClient::SyntheticsClient(const AWSCredentials& credentials,
                                   const Client::ClientConfiguration& clientConfiguration) :
  BASECLASS(clientConfiguration,
            Aws::MakeShared<AWSAuthV4Signer>(ALLOCATION_TAG,
                                             Aws::MakeShared<SimpleAWSCredentialsProvider>(ALLOCATION_TAG, credentials),
                                             SERVICE_NAME,
                                             Aws::Region::ComputeSignerRegion(clientConfiguration.region)),
            Aws::MakeShared<SyntheticsErrorMarshaller>(ALLOCATION_TAG)),
    m_clientConfiguration(clientConfiguration),
    m_executor(clientConfiguration.executor),
    m_endpointProvider(Aws::MakeShared<SyntheticsEndpointProvider>(ALLOCATION_TAG))
{
  init(m_clientConfiguration);
}

SyntheticsClient::SyntheticsClient(const std::shared_ptr<AWSCredentialsProvider>& credentialsProvider,
                                   const Client::ClientConfiguration& clientConfiguration) :
  BASECLASS(clientConfiguration,
            Aws::MakeShared<AWSAuthV4Signer>(ALLOCATION_TAG,
                                             credentialsProvider,
                                             SERVICE_NAME,
                                             Aws::Region::ComputeSignerRegion(clientConfiguration.region)),
            Aws::MakeShared<SyntheticsErrorMarshaller>(ALLOCATION_TAG)),
    m_clientConfiguration(clientConfiguration),
    m_executor(clientConfiguration.executor),
    m_endpointProvider(Aws::MakeShared<SyntheticsEndpointProvider>(ALLOCATION_TAG))
{
  init(m_clientConfiguration);
}

    /* End of legacy constructors due deprecation */
SyntheticsClient::~SyntheticsClient()
{
  ShutdownSdkClient(this, -1);
}

std::shared_ptr<SyntheticsEndpointProviderBase>& SyntheticsClient::accessEndpointProvider()
{
  return m_endpointProvider;
}

void SyntheticsClient::init(const Synthetics::SyntheticsClientConfiguration& config)
{
  AWSClient::SetServiceClientName("synthetics");
  AWS_CHECK_PTR(SERVICE_NAME, m_endpointProvider);
  m_endpointProvider->InitBuiltInParameters(config);
}

void SyntheticsClient::OverrideEndpoint(const Aws::String& endpoint)
{
  AWS_CHECK_PTR(SERVICE_NAME, m_endpointProvider);
  m_endpointProvider->OverrideEndpoint(endpoint);
}

AssociateResourceOutcome SyntheticsClient::AssociateResource(const AssociateResourceRequest& request) const
{
  AWS_OPERATION_GUARD(AssociateResource);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, AssociateResource, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  if (!request.GroupIdentifierHasBeenSet())
  {
    AWS_LOGSTREAM_ERROR("AssociateResource", "Required field: GroupIdentifier, is not set");
    return AssociateResourceOutcome(Aws::Client::AWSError<SyntheticsErrors>(SyntheticsErrors::MISSING_PARAMETER, "MISSING_PARAMETER", "Missing required field [GroupIdentifier]", false));
  }
  ResolveEndpointOutcome endpointResolutionOutcome = m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams());
  AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, AssociateResource, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
  endpointResolutionOutcome.GetResult().AddPathSegments("/group/");
  endpointResolutionOutcome.GetResult().AddPathSegment(request.GetGroupIdentifier());
  endpointResolutionOutcome.GetResult().AddPathSegments("/associate");
  return AssociateResourceOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_PATCH, Aws::Auth::SIGV4_SIGNER));
}

CreateCanaryOutcome SyntheticsClient::CreateCanary(const CreateCanaryRequest& request) const
{
  AWS_OPERATION_GUARD(CreateCanary);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, CreateCanary, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  ResolveEndpointOutcome endpointResolutionOutcome = m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams());
  AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, CreateCanary, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
  endpointResolutionOutcome.GetResult().AddPathSegments("/canary");
  return CreateCanaryOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_POST, Aws::Auth::SIGV4_SIGNER));
}

CreateGroupOutcome SyntheticsClient::CreateGroup(const CreateGroupRequest& request) const
{
  AWS_OPERATION_GUARD(CreateGroup);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, CreateGroup, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  ResolveEndpointOutcome endpointResolutionOutcome = m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams());
  AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, CreateGroup, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
  endpointResolutionOutcome.GetResult().AddPathSegments("/group");
  return CreateGroupOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_POST, Aws::Auth::SIGV4_SIGNER));
}

DeleteCanaryOutcome SyntheticsClient::DeleteCanary(const DeleteCanaryRequest& request) const
{
  AWS_OPERATION_GUARD(DeleteCanary);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, DeleteCanary, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  if (!request.NameHasBeenSet())
  {
    AWS_LOGSTREAM_ERROR("DeleteCanary", "Required field: Name, is not set");
    return DeleteCanaryOutcome(Aws::Client::AWSError<SyntheticsErrors>(SyntheticsErrors::MISSING_PARAMETER, "MISSING_PARAMETER", "Missing required field [Name]", false));
  }
  ResolveEndpointOutcome endpointResolutionOutcome = m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams());
  AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, DeleteCanary, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
  endpointResolutionOutcome.GetResult().AddPathSegments("/canary/");
  endpointResolutionOutcome.GetResult().AddPathSegment(request.GetName());
  return DeleteCanaryOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_DELETE, Aws::Auth::SIGV4_SIGNER));
}

DeleteGroupOutcome SyntheticsClient::DeleteGroup(const DeleteGroupRequest& request) const
{
  AWS_OPERATION_GUARD(DeleteGroup);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, DeleteGroup, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  if (!request.GroupIdentifierHasBeenSet())
  {
    AWS_LOGSTREAM_ERROR("DeleteGroup", "Required field: GroupIdentifier, is not set");
    return DeleteGroupOutcome(Aws::Client::AWSError<SyntheticsErrors>(SyntheticsErrors::MISSING_PARAMETER, "MISSING_PARAMETER", "Missing required field [GroupIdentifier]", false));
  }
  ResolveEndpointOutcome endpointResolutionOutcome = m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams());
  AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, DeleteGroup, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
  endpointResolutionOutcome.GetResult().AddPathSegments("/group/");
  endpointResolutionOutcome.GetResult().AddPathSegment(request.GetGroupIdentifier());
  return DeleteGroupOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_DELETE, Aws::Auth::SIGV4_SIGNER));
}

DescribeCanariesOutcome SyntheticsClient::DescribeCanaries(const DescribeCanariesRequest& request) const
{
  AWS_OPERATION_GUARD(DescribeCanaries);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, DescribeCanaries, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  ResolveEndpointOutcome endpointResolutionOutcome = m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams());
  AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, DescribeCanaries, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
  endpointResolutionOutcome.GetResult().AddPathSegments("/canaries");
  return DescribeCanariesOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_POST, Aws::Auth::SIGV4_SIGNER));
}

DescribeCanariesLastRunOutcome SyntheticsClient::DescribeCanariesLastRun(const DescribeCanariesLastRunRequest& request) const
{
  AWS_OPERATION_GUARD(DescribeCanariesLastRun);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, DescribeCanariesLastRun, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  ResolveEndpointOutcome endpointResolutionOutcome = m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams());
  AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, DescribeCanariesLastRun, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
  endpointResolutionOutcome.GetResult().AddPathSegments("/canaries/last-run");
  return DescribeCanariesLastRunOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_POST, Aws::Auth::SIGV4_SIGNER));
}

DescribeRuntimeVersionsOutcome SyntheticsClient::DescribeRuntimeVersions(const DescribeRuntimeVersionsRequest& request) const
{
  AWS_OPERATION_GUARD(DescribeRuntimeVersions);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, DescribeRuntimeVersions, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  ResolveEndpointOutcome endpointResolutionOutcome = m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams());
  AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, DescribeRuntimeVersions, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
  endpointResolutionOutcome.GetResult().AddPathSegments("/runtime-versions");
  return DescribeRuntimeVersionsOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_POST, Aws::Auth::SIGV4_SIGNER));
}

DisassociateResourceOutcome SyntheticsClient::DisassociateResource(const DisassociateResourceRequest& request) const
{
  AWS_OPERATION_GUARD(DisassociateResource);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, DisassociateResource, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  if (!request.GroupIdentifierHasBeenSet())
  {
    AWS_LOGSTREAM_ERROR("DisassociateResource", "Required field: GroupIdentifier, is not set");
    return DisassociateResourceOutcome(Aws::Client::AWSError<SyntheticsErrors>(SyntheticsErrors::MISSING_PARAMETER, "MISSING_PARAMETER", "Missing required field [GroupIdentifier]", false));
  }
  ResolveEndpointOutcome endpointResolutionOutcome = m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams());
  AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, DisassociateResource, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
  endpointResolutionOutcome.GetResult().AddPathSegments("/group/");
  endpointResolutionOutcome.GetResult().AddPathSegment(request.GetGroupIdentifier());
  endpointResolutionOutcome.GetResult().AddPathSegments("/disassociate");
  return DisassociateResourceOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_PATCH, Aws::Auth::SIGV4_SIGNER));
}

GetCanaryOutcome SyntheticsClient::GetCanary(const GetCanaryRequest& request) const
{
  AWS_OPERATION_GUARD(GetCanary);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, GetCanary, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  if (!request.NameHasBeenSet())
  {
    AWS_LOGSTREAM_ERROR("GetCanary", "Required field: Name, is not set");
    return GetCanaryOutcome(Aws::Client::AWSError<SyntheticsErrors>(SyntheticsErrors::MISSING_PARAMETER, "MISSING_PARAMETER", "Missing required field [Name]", false));
  }
  ResolveEndpointOutcome endpointResolutionOutcome = m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams());
  AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, GetCanary, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
  endpointResolutionOutcome.GetResult().AddPathSegments("/canary/");
  endpointResolutionOutcome.GetResult().AddPathSegment(request.GetName());
  return GetCanaryOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_GET, Aws::Auth::SIGV4_SIGNER));
}

GetCanaryRunsOutcome SyntheticsClient::GetCanaryRuns(const GetCanaryRunsRequest& request) const
{
  AWS_OPERATION_GUARD(GetCanaryRuns);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, GetCanaryRuns, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  if (!request.NameHasBeenSet())
  {
    AWS_LOGSTREAM_ERROR("GetCanaryRuns", "Required field: Name, is not set");
    return GetCanaryRunsOutcome(Aws::Client::AWSError<SyntheticsErrors>(SyntheticsErrors::MISSING_PARAMETER, "MISSING_PARAMETER", "Missing required field [Name]", false));
  }
  ResolveEndpointOutcome endpointResolutionOutcome = m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams());
  AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, GetCanaryRuns, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
  endpointResolutionOutcome.GetResult().AddPathSegments("/canary/");
  endpointResolutionOutcome.GetResult().AddPathSegment(request.GetName());
  endpointResolutionOutcome.GetResult().AddPathSegments("/runs");
  return GetCanaryRunsOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_POST, Aws::Auth::SIGV4_SIGNER));
}

GetGroupOutcome SyntheticsClient::GetGroup(const GetGroupRequest& request) const
{
  AWS_OPERATION_GUARD(GetGroup);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, GetGroup, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  if (!request.GroupIdentifierHasBeenSet())
  {
    AWS_LOGSTREAM_ERROR("GetGroup", "Required field: GroupIdentifier, is not set");
    return GetGroupOutcome(Aws::Client::AWSError<SyntheticsErrors>(SyntheticsErrors::MISSING_PARAMETER, "MISSING_PARAMETER", "Missing required field [GroupIdentifier]", false));
  }
  ResolveEndpointOutcome endpointResolutionOutcome = m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams());
  AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, GetGroup, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
  endpointResolutionOutcome.GetResult().AddPathSegments("/group/");
  endpointResolutionOutcome.GetResult().AddPathSegment(request.GetGroupIdentifier());
  return GetGroupOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_GET, Aws::Auth::SIGV4_SIGNER));
}

ListAssociatedGroupsOutcome SyntheticsClient::ListAssociatedGroups(const ListAssociatedGroupsRequest& request) const
{
  AWS_OPERATION_GUARD(ListAssociatedGroups);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, ListAssociatedGroups, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  if (!request.ResourceArnHasBeenSet())
  {
    AWS_LOGSTREAM_ERROR("ListAssociatedGroups", "Required field: ResourceArn, is not set");
    return ListAssociatedGroupsOutcome(Aws::Client::AWSError<SyntheticsErrors>(SyntheticsErrors::MISSING_PARAMETER, "MISSING_PARAMETER", "Missing required field [ResourceArn]", false));
  }
  ResolveEndpointOutcome endpointResolutionOutcome = m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams());
  AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, ListAssociatedGroups, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
  endpointResolutionOutcome.GetResult().AddPathSegments("/resource/");
  endpointResolutionOutcome.GetResult().AddPathSegment(request.GetResourceArn());
  endpointResolutionOutcome.GetResult().AddPathSegments("/groups");
  return ListAssociatedGroupsOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_POST, Aws::Auth::SIGV4_SIGNER));
}

ListGroupResourcesOutcome SyntheticsClient::ListGroupResources(const ListGroupResourcesRequest& request) const
{
  AWS_OPERATION_GUARD(ListGroupResources);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, ListGroupResources, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  if (!request.GroupIdentifierHasBeenSet())
  {
    AWS_LOGSTREAM_ERROR("ListGroupResources", "Required field: GroupIdentifier, is not set");
    return ListGroupResourcesOutcome(Aws::Client::AWSError<SyntheticsErrors>(SyntheticsErrors::MISSING_PARAMETER, "MISSING_PARAMETER", "Missing required field [GroupIdentifier]", false));
  }
  ResolveEndpointOutcome endpointResolutionOutcome = m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams());
  AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, ListGroupResources, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
  endpointResolutionOutcome.GetResult().AddPathSegments("/group/");
  endpointResolutionOutcome.GetResult().AddPathSegment(request.GetGroupIdentifier());
  endpointResolutionOutcome.GetResult().AddPathSegments("/resources");
  return ListGroupResourcesOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_POST, Aws::Auth::SIGV4_SIGNER));
}

ListGroupsOutcome SyntheticsClient::ListGroups(const ListGroupsRequest& request) const
{
  AWS_OPERATION_GUARD(ListGroups);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, ListGroups, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  ResolveEndpointOutcome endpointResolutionOutcome = m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams());
  AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, ListGroups, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
  endpointResolutionOutcome.GetResult().AddPathSegments("/groups");
  return ListGroupsOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_POST, Aws::Auth::SIGV4_SIGNER));
}

ListTagsForResourceOutcome SyntheticsClient::ListTagsForResource(const ListTagsForResourceRequest& request) const
{
  AWS_OPERATION_GUARD(ListTagsForResource);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, ListTagsForResource, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  if (!request.ResourceArnHasBeenSet())
  {
    AWS_LOGSTREAM_ERROR("ListTagsForResource", "Required field: ResourceArn, is not set");
    return ListTagsForResourceOutcome(Aws::Client::AWSError<SyntheticsErrors>(SyntheticsErrors::MISSING_PARAMETER, "MISSING_PARAMETER", "Missing required field [ResourceArn]", false));
  }
  ResolveEndpointOutcome endpointResolutionOutcome = m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams());
  AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, ListTagsForResource, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
  endpointResolutionOutcome.GetResult().AddPathSegments("/tags/");
  endpointResolutionOutcome.GetResult().AddPathSegment(request.GetResourceArn());
  return ListTagsForResourceOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_GET, Aws::Auth::SIGV4_SIGNER));
}

StartCanaryOutcome SyntheticsClient::StartCanary(const StartCanaryRequest& request) const
{
  AWS_OPERATION_GUARD(StartCanary);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, StartCanary, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  if (!request.NameHasBeenSet())
  {
    AWS_LOGSTREAM_ERROR("StartCanary", "Required field: Name, is not set");
    return StartCanaryOutcome(Aws::Client::AWSError<SyntheticsErrors>(SyntheticsErrors::MISSING_PARAMETER, "MISSING_PARAMETER", "Missing required field [Name]", false));
  }
  ResolveEndpointOutcome endpointResolutionOutcome = m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams());
  AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, StartCanary, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
  endpointResolutionOutcome.GetResult().AddPathSegments("/canary/");
  endpointResolutionOutcome.GetResult().AddPathSegment(request.GetName());
  endpointResolutionOutcome.GetResult().AddPathSegments("/start");
  return StartCanaryOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_POST, Aws::Auth::SIGV4_SIGNER));
}

StopCanaryOutcome SyntheticsClient::StopCanary(const StopCanaryRequest& request) const
{
  AWS_OPERATION_GUARD(StopCanary);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, StopCanary, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  if (!request.NameHasBeenSet())
  {
    AWS_LOGSTREAM_ERROR("StopCanary", "Required field: Name, is not set");
    return StopCanaryOutcome(Aws::Client::AWSError<SyntheticsErrors>(SyntheticsErrors::MISSING_PARAMETER, "MISSING_PARAMETER", "Missing required field [Name]", false));
  }
  ResolveEndpointOutcome endpointResolutionOutcome = m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams());
  AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, StopCanary, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
  endpointResolutionOutcome.GetResult().AddPathSegments("/canary/");
  endpointResolutionOutcome.GetResult().AddPathSegment(request.GetName());
  endpointResolutionOutcome.GetResult().AddPathSegments("/stop");
  return StopCanaryOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_POST, Aws::Auth::SIGV4_SIGNER));
}

TagResourceOutcome SyntheticsClient::TagResource(const TagResourceRequest& request) const
{
  AWS_OPERATION_GUARD(TagResource);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, TagResource, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  if (!request.ResourceArnHasBeenSet())
  {
    AWS_LOGSTREAM_ERROR("TagResource", "Required field: ResourceArn, is not set");
    return TagResourceOutcome(Aws::Client::AWSError<SyntheticsErrors>(SyntheticsErrors::MISSING_PARAMETER, "MISSING_PARAMETER", "Missing required field [ResourceArn]", false));
  }
  ResolveEndpointOutcome endpointResolutionOutcome = m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams());
  AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, TagResource, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
  endpointResolutionOutcome.GetResult().AddPathSegments("/tags/");
  endpointResolutionOutcome.GetResult().AddPathSegment(request.GetResourceArn());
  return TagResourceOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_POST, Aws::Auth::SIGV4_SIGNER));
}

UntagResourceOutcome SyntheticsClient::UntagResource(const UntagResourceRequest& request) const
{
  AWS_OPERATION_GUARD(UntagResource);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, UntagResource, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  if (!request.ResourceArnHasBeenSet())
  {
    AWS_LOGSTREAM_ERROR("UntagResource", "Required field: ResourceArn, is not set");
    return UntagResourceOutcome(Aws::Client::AWSError<SyntheticsErrors>(SyntheticsErrors::MISSING_PARAMETER, "MISSING_PARAMETER", "Missing required field [ResourceArn]", false));
  }
  if (!request.TagKeysHasBeenSet())
  {
    AWS_LOGSTREAM_ERROR("UntagResource", "Required field: TagKeys, is not set");
    return UntagResourceOutcome(Aws::Client::AWSError<SyntheticsErrors>(SyntheticsErrors::MISSING_PARAMETER, "MISSING_PARAMETER", "Missing required field [TagKeys]", false));
  }
  ResolveEndpointOutcome endpointResolutionOutcome = m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams());
  AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, UntagResource, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
  endpointResolutionOutcome.GetResult().AddPathSegments("/tags/");
  endpointResolutionOutcome.GetResult().AddPathSegment(request.GetResourceArn());
  return UntagResourceOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_DELETE, Aws::Auth::SIGV4_SIGNER));
}

UpdateCanaryOutcome SyntheticsClient::UpdateCanary(const UpdateCanaryRequest& request) const
{
  AWS_OPERATION_GUARD(UpdateCanary);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, UpdateCanary, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  if (!request.NameHasBeenSet())
  {
    AWS_LOGSTREAM_ERROR("UpdateCanary", "Required field: Name, is not set");
    return UpdateCanaryOutcome(Aws::Client::AWSError<SyntheticsErrors>(SyntheticsErrors::MISSING_PARAMETER, "MISSING_PARAMETER", "Missing required field [Name]", false));
  }
  ResolveEndpointOutcome endpointResolutionOutcome = m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams());
  AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, UpdateCanary, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
  endpointResolutionOutcome.GetResult().AddPathSegments("/canary/");
  endpointResolutionOutcome.GetResult().AddPathSegment(request.GetName());
  return UpdateCanaryOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_PATCH, Aws::Auth::SIGV4_SIGNER));
}

