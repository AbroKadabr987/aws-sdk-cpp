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

#include <aws/iot-roborunner/IoTRoboRunnerClient.h>
#include <aws/iot-roborunner/IoTRoboRunnerErrorMarshaller.h>
#include <aws/iot-roborunner/IoTRoboRunnerEndpointProvider.h>
#include <aws/iot-roborunner/model/CreateDestinationRequest.h>
#include <aws/iot-roborunner/model/CreateSiteRequest.h>
#include <aws/iot-roborunner/model/CreateWorkerRequest.h>
#include <aws/iot-roborunner/model/CreateWorkerFleetRequest.h>
#include <aws/iot-roborunner/model/DeleteDestinationRequest.h>
#include <aws/iot-roborunner/model/DeleteSiteRequest.h>
#include <aws/iot-roborunner/model/DeleteWorkerRequest.h>
#include <aws/iot-roborunner/model/DeleteWorkerFleetRequest.h>
#include <aws/iot-roborunner/model/GetDestinationRequest.h>
#include <aws/iot-roborunner/model/GetSiteRequest.h>
#include <aws/iot-roborunner/model/GetWorkerRequest.h>
#include <aws/iot-roborunner/model/GetWorkerFleetRequest.h>
#include <aws/iot-roborunner/model/ListDestinationsRequest.h>
#include <aws/iot-roborunner/model/ListSitesRequest.h>
#include <aws/iot-roborunner/model/ListWorkerFleetsRequest.h>
#include <aws/iot-roborunner/model/ListWorkersRequest.h>
#include <aws/iot-roborunner/model/UpdateDestinationRequest.h>
#include <aws/iot-roborunner/model/UpdateSiteRequest.h>
#include <aws/iot-roborunner/model/UpdateWorkerRequest.h>
#include <aws/iot-roborunner/model/UpdateWorkerFleetRequest.h>

using namespace Aws;
using namespace Aws::Auth;
using namespace Aws::Client;
using namespace Aws::IoTRoboRunner;
using namespace Aws::IoTRoboRunner::Model;
using namespace Aws::Http;
using namespace Aws::Utils::Json;
using ResolveEndpointOutcome = Aws::Endpoint::ResolveEndpointOutcome;

const char* IoTRoboRunnerClient::SERVICE_NAME = "iotroborunner";
const char* IoTRoboRunnerClient::ALLOCATION_TAG = "IoTRoboRunnerClient";

IoTRoboRunnerClient::IoTRoboRunnerClient(const IoTRoboRunner::IoTRoboRunnerClientConfiguration& clientConfiguration,
                                         std::shared_ptr<IoTRoboRunnerEndpointProviderBase> endpointProvider) :
  BASECLASS(clientConfiguration,
            Aws::MakeShared<AWSAuthV4Signer>(ALLOCATION_TAG,
                                             Aws::MakeShared<DefaultAWSCredentialsProviderChain>(ALLOCATION_TAG),
                                             SERVICE_NAME,
                                             Aws::Region::ComputeSignerRegion(clientConfiguration.region)),
            Aws::MakeShared<IoTRoboRunnerErrorMarshaller>(ALLOCATION_TAG)),
  m_clientConfiguration(clientConfiguration),
  m_executor(clientConfiguration.executor),
  m_endpointProvider(std::move(endpointProvider))
{
  init(m_clientConfiguration);
}

IoTRoboRunnerClient::IoTRoboRunnerClient(const AWSCredentials& credentials,
                                         std::shared_ptr<IoTRoboRunnerEndpointProviderBase> endpointProvider,
                                         const IoTRoboRunner::IoTRoboRunnerClientConfiguration& clientConfiguration) :
  BASECLASS(clientConfiguration,
            Aws::MakeShared<AWSAuthV4Signer>(ALLOCATION_TAG,
                                             Aws::MakeShared<SimpleAWSCredentialsProvider>(ALLOCATION_TAG, credentials),
                                             SERVICE_NAME,
                                             Aws::Region::ComputeSignerRegion(clientConfiguration.region)),
            Aws::MakeShared<IoTRoboRunnerErrorMarshaller>(ALLOCATION_TAG)),
    m_clientConfiguration(clientConfiguration),
    m_executor(clientConfiguration.executor),
    m_endpointProvider(std::move(endpointProvider))
{
  init(m_clientConfiguration);
}

IoTRoboRunnerClient::IoTRoboRunnerClient(const std::shared_ptr<AWSCredentialsProvider>& credentialsProvider,
                                         std::shared_ptr<IoTRoboRunnerEndpointProviderBase> endpointProvider,
                                         const IoTRoboRunner::IoTRoboRunnerClientConfiguration& clientConfiguration) :
  BASECLASS(clientConfiguration,
            Aws::MakeShared<AWSAuthV4Signer>(ALLOCATION_TAG,
                                             credentialsProvider,
                                             SERVICE_NAME,
                                             Aws::Region::ComputeSignerRegion(clientConfiguration.region)),
            Aws::MakeShared<IoTRoboRunnerErrorMarshaller>(ALLOCATION_TAG)),
    m_clientConfiguration(clientConfiguration),
    m_executor(clientConfiguration.executor),
    m_endpointProvider(std::move(endpointProvider))
{
  init(m_clientConfiguration);
}

    /* Legacy constructors due deprecation */
  IoTRoboRunnerClient::IoTRoboRunnerClient(const Client::ClientConfiguration& clientConfiguration) :
  BASECLASS(clientConfiguration,
            Aws::MakeShared<AWSAuthV4Signer>(ALLOCATION_TAG,
                                             Aws::MakeShared<DefaultAWSCredentialsProviderChain>(ALLOCATION_TAG),
                                             SERVICE_NAME,
                                             Aws::Region::ComputeSignerRegion(clientConfiguration.region)),
            Aws::MakeShared<IoTRoboRunnerErrorMarshaller>(ALLOCATION_TAG)),
  m_clientConfiguration(clientConfiguration),
  m_executor(clientConfiguration.executor),
  m_endpointProvider(Aws::MakeShared<IoTRoboRunnerEndpointProvider>(ALLOCATION_TAG))
{
  init(m_clientConfiguration);
}

IoTRoboRunnerClient::IoTRoboRunnerClient(const AWSCredentials& credentials,
                                         const Client::ClientConfiguration& clientConfiguration) :
  BASECLASS(clientConfiguration,
            Aws::MakeShared<AWSAuthV4Signer>(ALLOCATION_TAG,
                                             Aws::MakeShared<SimpleAWSCredentialsProvider>(ALLOCATION_TAG, credentials),
                                             SERVICE_NAME,
                                             Aws::Region::ComputeSignerRegion(clientConfiguration.region)),
            Aws::MakeShared<IoTRoboRunnerErrorMarshaller>(ALLOCATION_TAG)),
    m_clientConfiguration(clientConfiguration),
    m_executor(clientConfiguration.executor),
    m_endpointProvider(Aws::MakeShared<IoTRoboRunnerEndpointProvider>(ALLOCATION_TAG))
{
  init(m_clientConfiguration);
}

IoTRoboRunnerClient::IoTRoboRunnerClient(const std::shared_ptr<AWSCredentialsProvider>& credentialsProvider,
                                         const Client::ClientConfiguration& clientConfiguration) :
  BASECLASS(clientConfiguration,
            Aws::MakeShared<AWSAuthV4Signer>(ALLOCATION_TAG,
                                             credentialsProvider,
                                             SERVICE_NAME,
                                             Aws::Region::ComputeSignerRegion(clientConfiguration.region)),
            Aws::MakeShared<IoTRoboRunnerErrorMarshaller>(ALLOCATION_TAG)),
    m_clientConfiguration(clientConfiguration),
    m_executor(clientConfiguration.executor),
    m_endpointProvider(Aws::MakeShared<IoTRoboRunnerEndpointProvider>(ALLOCATION_TAG))
{
  init(m_clientConfiguration);
}

    /* End of legacy constructors due deprecation */
IoTRoboRunnerClient::~IoTRoboRunnerClient()
{
  ShutdownSdkClient(this, -1);
}

std::shared_ptr<IoTRoboRunnerEndpointProviderBase>& IoTRoboRunnerClient::accessEndpointProvider()
{
  return m_endpointProvider;
}

void IoTRoboRunnerClient::init(const IoTRoboRunner::IoTRoboRunnerClientConfiguration& config)
{
  AWSClient::SetServiceClientName("IoT RoboRunner");
  AWS_CHECK_PTR(SERVICE_NAME, m_endpointProvider);
  m_endpointProvider->InitBuiltInParameters(config);
}

void IoTRoboRunnerClient::OverrideEndpoint(const Aws::String& endpoint)
{
  AWS_CHECK_PTR(SERVICE_NAME, m_endpointProvider);
  m_endpointProvider->OverrideEndpoint(endpoint);
}

CreateDestinationOutcome IoTRoboRunnerClient::CreateDestination(const CreateDestinationRequest& request) const
{
  AWS_OPERATION_GUARD(CreateDestination);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, CreateDestination, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  ResolveEndpointOutcome endpointResolutionOutcome = m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams());
  AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, CreateDestination, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
  endpointResolutionOutcome.GetResult().AddPathSegments("/createDestination");
  return CreateDestinationOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_POST, Aws::Auth::SIGV4_SIGNER));
}

CreateSiteOutcome IoTRoboRunnerClient::CreateSite(const CreateSiteRequest& request) const
{
  AWS_OPERATION_GUARD(CreateSite);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, CreateSite, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  ResolveEndpointOutcome endpointResolutionOutcome = m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams());
  AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, CreateSite, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
  endpointResolutionOutcome.GetResult().AddPathSegments("/createSite");
  return CreateSiteOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_POST, Aws::Auth::SIGV4_SIGNER));
}

CreateWorkerOutcome IoTRoboRunnerClient::CreateWorker(const CreateWorkerRequest& request) const
{
  AWS_OPERATION_GUARD(CreateWorker);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, CreateWorker, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  ResolveEndpointOutcome endpointResolutionOutcome = m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams());
  AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, CreateWorker, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
  endpointResolutionOutcome.GetResult().AddPathSegments("/createWorker");
  return CreateWorkerOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_POST, Aws::Auth::SIGV4_SIGNER));
}

CreateWorkerFleetOutcome IoTRoboRunnerClient::CreateWorkerFleet(const CreateWorkerFleetRequest& request) const
{
  AWS_OPERATION_GUARD(CreateWorkerFleet);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, CreateWorkerFleet, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  ResolveEndpointOutcome endpointResolutionOutcome = m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams());
  AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, CreateWorkerFleet, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
  endpointResolutionOutcome.GetResult().AddPathSegments("/createWorkerFleet");
  return CreateWorkerFleetOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_POST, Aws::Auth::SIGV4_SIGNER));
}

DeleteDestinationOutcome IoTRoboRunnerClient::DeleteDestination(const DeleteDestinationRequest& request) const
{
  AWS_OPERATION_GUARD(DeleteDestination);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, DeleteDestination, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  ResolveEndpointOutcome endpointResolutionOutcome = m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams());
  AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, DeleteDestination, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
  endpointResolutionOutcome.GetResult().AddPathSegments("/deleteDestination");
  return DeleteDestinationOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_POST, Aws::Auth::SIGV4_SIGNER));
}

DeleteSiteOutcome IoTRoboRunnerClient::DeleteSite(const DeleteSiteRequest& request) const
{
  AWS_OPERATION_GUARD(DeleteSite);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, DeleteSite, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  ResolveEndpointOutcome endpointResolutionOutcome = m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams());
  AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, DeleteSite, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
  endpointResolutionOutcome.GetResult().AddPathSegments("/deleteSite");
  return DeleteSiteOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_POST, Aws::Auth::SIGV4_SIGNER));
}

DeleteWorkerOutcome IoTRoboRunnerClient::DeleteWorker(const DeleteWorkerRequest& request) const
{
  AWS_OPERATION_GUARD(DeleteWorker);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, DeleteWorker, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  ResolveEndpointOutcome endpointResolutionOutcome = m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams());
  AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, DeleteWorker, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
  endpointResolutionOutcome.GetResult().AddPathSegments("/deleteWorker");
  return DeleteWorkerOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_POST, Aws::Auth::SIGV4_SIGNER));
}

DeleteWorkerFleetOutcome IoTRoboRunnerClient::DeleteWorkerFleet(const DeleteWorkerFleetRequest& request) const
{
  AWS_OPERATION_GUARD(DeleteWorkerFleet);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, DeleteWorkerFleet, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  ResolveEndpointOutcome endpointResolutionOutcome = m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams());
  AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, DeleteWorkerFleet, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
  endpointResolutionOutcome.GetResult().AddPathSegments("/deleteWorkerFleet");
  return DeleteWorkerFleetOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_POST, Aws::Auth::SIGV4_SIGNER));
}

GetDestinationOutcome IoTRoboRunnerClient::GetDestination(const GetDestinationRequest& request) const
{
  AWS_OPERATION_GUARD(GetDestination);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, GetDestination, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  if (!request.IdHasBeenSet())
  {
    AWS_LOGSTREAM_ERROR("GetDestination", "Required field: Id, is not set");
    return GetDestinationOutcome(Aws::Client::AWSError<IoTRoboRunnerErrors>(IoTRoboRunnerErrors::MISSING_PARAMETER, "MISSING_PARAMETER", "Missing required field [Id]", false));
  }
  ResolveEndpointOutcome endpointResolutionOutcome = m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams());
  AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, GetDestination, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
  endpointResolutionOutcome.GetResult().AddPathSegments("/getDestination");
  return GetDestinationOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_GET, Aws::Auth::SIGV4_SIGNER));
}

GetSiteOutcome IoTRoboRunnerClient::GetSite(const GetSiteRequest& request) const
{
  AWS_OPERATION_GUARD(GetSite);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, GetSite, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  if (!request.IdHasBeenSet())
  {
    AWS_LOGSTREAM_ERROR("GetSite", "Required field: Id, is not set");
    return GetSiteOutcome(Aws::Client::AWSError<IoTRoboRunnerErrors>(IoTRoboRunnerErrors::MISSING_PARAMETER, "MISSING_PARAMETER", "Missing required field [Id]", false));
  }
  ResolveEndpointOutcome endpointResolutionOutcome = m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams());
  AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, GetSite, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
  endpointResolutionOutcome.GetResult().AddPathSegments("/getSite");
  return GetSiteOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_GET, Aws::Auth::SIGV4_SIGNER));
}

GetWorkerOutcome IoTRoboRunnerClient::GetWorker(const GetWorkerRequest& request) const
{
  AWS_OPERATION_GUARD(GetWorker);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, GetWorker, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  if (!request.IdHasBeenSet())
  {
    AWS_LOGSTREAM_ERROR("GetWorker", "Required field: Id, is not set");
    return GetWorkerOutcome(Aws::Client::AWSError<IoTRoboRunnerErrors>(IoTRoboRunnerErrors::MISSING_PARAMETER, "MISSING_PARAMETER", "Missing required field [Id]", false));
  }
  ResolveEndpointOutcome endpointResolutionOutcome = m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams());
  AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, GetWorker, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
  endpointResolutionOutcome.GetResult().AddPathSegments("/getWorker");
  return GetWorkerOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_GET, Aws::Auth::SIGV4_SIGNER));
}

GetWorkerFleetOutcome IoTRoboRunnerClient::GetWorkerFleet(const GetWorkerFleetRequest& request) const
{
  AWS_OPERATION_GUARD(GetWorkerFleet);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, GetWorkerFleet, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  if (!request.IdHasBeenSet())
  {
    AWS_LOGSTREAM_ERROR("GetWorkerFleet", "Required field: Id, is not set");
    return GetWorkerFleetOutcome(Aws::Client::AWSError<IoTRoboRunnerErrors>(IoTRoboRunnerErrors::MISSING_PARAMETER, "MISSING_PARAMETER", "Missing required field [Id]", false));
  }
  ResolveEndpointOutcome endpointResolutionOutcome = m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams());
  AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, GetWorkerFleet, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
  endpointResolutionOutcome.GetResult().AddPathSegments("/getWorkerFleet");
  return GetWorkerFleetOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_GET, Aws::Auth::SIGV4_SIGNER));
}

ListDestinationsOutcome IoTRoboRunnerClient::ListDestinations(const ListDestinationsRequest& request) const
{
  AWS_OPERATION_GUARD(ListDestinations);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, ListDestinations, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  if (!request.SiteHasBeenSet())
  {
    AWS_LOGSTREAM_ERROR("ListDestinations", "Required field: Site, is not set");
    return ListDestinationsOutcome(Aws::Client::AWSError<IoTRoboRunnerErrors>(IoTRoboRunnerErrors::MISSING_PARAMETER, "MISSING_PARAMETER", "Missing required field [Site]", false));
  }
  ResolveEndpointOutcome endpointResolutionOutcome = m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams());
  AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, ListDestinations, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
  endpointResolutionOutcome.GetResult().AddPathSegments("/listDestinations");
  return ListDestinationsOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_GET, Aws::Auth::SIGV4_SIGNER));
}

ListSitesOutcome IoTRoboRunnerClient::ListSites(const ListSitesRequest& request) const
{
  AWS_OPERATION_GUARD(ListSites);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, ListSites, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  ResolveEndpointOutcome endpointResolutionOutcome = m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams());
  AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, ListSites, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
  endpointResolutionOutcome.GetResult().AddPathSegments("/listSites");
  return ListSitesOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_GET, Aws::Auth::SIGV4_SIGNER));
}

ListWorkerFleetsOutcome IoTRoboRunnerClient::ListWorkerFleets(const ListWorkerFleetsRequest& request) const
{
  AWS_OPERATION_GUARD(ListWorkerFleets);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, ListWorkerFleets, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  if (!request.SiteHasBeenSet())
  {
    AWS_LOGSTREAM_ERROR("ListWorkerFleets", "Required field: Site, is not set");
    return ListWorkerFleetsOutcome(Aws::Client::AWSError<IoTRoboRunnerErrors>(IoTRoboRunnerErrors::MISSING_PARAMETER, "MISSING_PARAMETER", "Missing required field [Site]", false));
  }
  ResolveEndpointOutcome endpointResolutionOutcome = m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams());
  AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, ListWorkerFleets, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
  endpointResolutionOutcome.GetResult().AddPathSegments("/listWorkerFleets");
  return ListWorkerFleetsOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_GET, Aws::Auth::SIGV4_SIGNER));
}

ListWorkersOutcome IoTRoboRunnerClient::ListWorkers(const ListWorkersRequest& request) const
{
  AWS_OPERATION_GUARD(ListWorkers);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, ListWorkers, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  if (!request.SiteHasBeenSet())
  {
    AWS_LOGSTREAM_ERROR("ListWorkers", "Required field: Site, is not set");
    return ListWorkersOutcome(Aws::Client::AWSError<IoTRoboRunnerErrors>(IoTRoboRunnerErrors::MISSING_PARAMETER, "MISSING_PARAMETER", "Missing required field [Site]", false));
  }
  ResolveEndpointOutcome endpointResolutionOutcome = m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams());
  AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, ListWorkers, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
  endpointResolutionOutcome.GetResult().AddPathSegments("/listWorkers");
  return ListWorkersOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_GET, Aws::Auth::SIGV4_SIGNER));
}

UpdateDestinationOutcome IoTRoboRunnerClient::UpdateDestination(const UpdateDestinationRequest& request) const
{
  AWS_OPERATION_GUARD(UpdateDestination);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, UpdateDestination, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  ResolveEndpointOutcome endpointResolutionOutcome = m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams());
  AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, UpdateDestination, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
  endpointResolutionOutcome.GetResult().AddPathSegments("/updateDestination");
  return UpdateDestinationOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_POST, Aws::Auth::SIGV4_SIGNER));
}

UpdateSiteOutcome IoTRoboRunnerClient::UpdateSite(const UpdateSiteRequest& request) const
{
  AWS_OPERATION_GUARD(UpdateSite);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, UpdateSite, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  ResolveEndpointOutcome endpointResolutionOutcome = m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams());
  AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, UpdateSite, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
  endpointResolutionOutcome.GetResult().AddPathSegments("/updateSite");
  return UpdateSiteOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_POST, Aws::Auth::SIGV4_SIGNER));
}

UpdateWorkerOutcome IoTRoboRunnerClient::UpdateWorker(const UpdateWorkerRequest& request) const
{
  AWS_OPERATION_GUARD(UpdateWorker);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, UpdateWorker, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  ResolveEndpointOutcome endpointResolutionOutcome = m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams());
  AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, UpdateWorker, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
  endpointResolutionOutcome.GetResult().AddPathSegments("/updateWorker");
  return UpdateWorkerOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_POST, Aws::Auth::SIGV4_SIGNER));
}

UpdateWorkerFleetOutcome IoTRoboRunnerClient::UpdateWorkerFleet(const UpdateWorkerFleetRequest& request) const
{
  AWS_OPERATION_GUARD(UpdateWorkerFleet);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, UpdateWorkerFleet, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  ResolveEndpointOutcome endpointResolutionOutcome = m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams());
  AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, UpdateWorkerFleet, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
  endpointResolutionOutcome.GetResult().AddPathSegments("/updateWorkerFleet");
  return UpdateWorkerFleetOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_POST, Aws::Auth::SIGV4_SIGNER));
}

