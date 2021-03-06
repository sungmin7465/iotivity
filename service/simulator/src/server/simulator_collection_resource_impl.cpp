/******************************************************************
 *
 * Copyright 2015 Samsung Electronics All Rights Reserved.
 *
 *
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 ******************************************************************/

#include "simulator_collection_resource_impl.h"
#include "simulator_utils.h"
#include "simulator_logger.h"
#include "logger.h"

#define TAG "SIM_COLLECTION_RESOURCE"

SimulatorCollectionResourceImpl::SimulatorCollectionResourceImpl()
    :   m_type(SimulatorResource::Type::COLLECTION_RESOURCE),
        m_interfaces {OC::DEFAULT_INTERFACE, OC::LINK_INTERFACE},
        m_resourceHandle(NULL)
{
    m_property = static_cast<OCResourceProperty>(OC_DISCOVERABLE | OC_OBSERVABLE);

    std::vector<SimulatorResourceModel> links;
    m_resModel.add("links", links);
}

std::string SimulatorCollectionResourceImpl::getName() const
{
    return m_name;
}

SimulatorResource::Type SimulatorCollectionResourceImpl::getType() const
{
    return m_type;
}

std::string SimulatorCollectionResourceImpl::getURI() const
{
    return m_uri;
}

std::string SimulatorCollectionResourceImpl::getResourceType() const
{
    return m_resourceType;
}

std::vector<std::string> SimulatorCollectionResourceImpl::getInterface() const
{
    return m_interfaces;
}

void SimulatorCollectionResourceImpl::setInterface(const std::vector<std::string> &interfaces)
{
    m_interfaces = interfaces;
}

void SimulatorCollectionResourceImpl::setName(const std::string &name)
{
    VALIDATE_INPUT(name.empty(), "Name is empty!")

    std::lock_guard<std::recursive_mutex> lock(m_objectLock);
    if (m_resourceHandle)
    {
        throw SimulatorException(SIMULATOR_OPERATION_NOT_ALLOWED,
                                 "Name can not be set when collection is started!");
    }

    m_name = name;
}

void SimulatorCollectionResourceImpl::setURI(const std::string &uri)
{
    VALIDATE_INPUT(uri.empty(), "Uri is empty!")

    std::lock_guard<std::recursive_mutex> lock(m_objectLock);
    if (m_resourceHandle)
    {
        throw SimulatorException(SIMULATOR_OPERATION_NOT_ALLOWED,
                                 "URI can not be set when collection is started!");
    }

    m_uri = uri;
}

void SimulatorCollectionResourceImpl::setResourceType(const std::string &resourceType)
{
    VALIDATE_INPUT(resourceType.empty(), "Resource type is empty!")

    std::lock_guard<std::recursive_mutex> lock(m_objectLock);
    if (m_resourceHandle)
    {
        throw SimulatorException(SIMULATOR_OPERATION_NOT_ALLOWED,
                                 "Resource type can not be set when collection is started!");
    }

    m_resourceType = resourceType;
}

void SimulatorCollectionResourceImpl::addInterface(std::string interfaceType)
{
    VALIDATE_INPUT(interfaceType.empty(), "Interface type is empty!")

    if (interfaceType == OC::GROUP_INTERFACE)
    {
        throw NoSupportException("Collection resource does not support this interface type!");
    }

    std::lock_guard<std::recursive_mutex> lock(m_objectLock);
    if (m_resourceHandle)
    {
        throw SimulatorException(SIMULATOR_OPERATION_NOT_ALLOWED,
                                 "Interface type can not be set when resource is started!");
    }

    auto found = std::find(m_interfaces.begin(), m_interfaces.end(), interfaceType);
    if (found != m_interfaces.end())
        m_interfaces.push_back(interfaceType);
}

void SimulatorCollectionResourceImpl::setObservable(bool state)
{
    std::lock_guard<std::recursive_mutex> lock(m_objectLock);
    if (m_resourceHandle)
    {
        throw SimulatorException(SIMULATOR_OPERATION_NOT_ALLOWED,
                                 "Observation state can not be changed when resource is started!");
    }

    if (true == state)
        m_property = static_cast<OCResourceProperty>(m_property | OC_OBSERVABLE);
    else
        m_property = static_cast<OCResourceProperty>(m_property ^ OC_OBSERVABLE);
}

void SimulatorCollectionResourceImpl::setObserverCallback(ObserverCallback callback)
{
    VALIDATE_CALLBACK(callback)
    m_observeCallback = callback;
}

void SimulatorCollectionResourceImpl::setModelChangeCallback(ResourceModelChangedCallback callback)
{
    VALIDATE_CALLBACK(callback)
    m_modelCallback = callback;
}

bool SimulatorCollectionResourceImpl::isObservable()
{
    return (m_property & OC_OBSERVABLE);
}

bool SimulatorCollectionResourceImpl::isStarted()
{
    return (nullptr != m_resourceHandle);
}

void SimulatorCollectionResourceImpl::start()
{
    std::lock_guard<std::recursive_mutex> lock(m_objectLock);
    if (m_resourceHandle)
    {
        throw SimulatorException(SIMULATOR_ERROR, "Collection already registered!");
    }

    if (m_uri.empty() || m_resourceType.empty())
    {
        throw SimulatorException(SIMULATOR_ERROR, "Found incomplete data to start resource!");
    }

    typedef OCStackResult (*RegisterResource)(OCResourceHandle &, std::string &, const std::string &,
            const std::string &, OC::EntityHandler, uint8_t);

    invokeocplatform(static_cast<RegisterResource>(OC::OCPlatform::registerResource),
                     m_resourceHandle, m_uri, m_resourceType, m_interfaces[0],
                     std::bind(&SimulatorCollectionResourceImpl::handleRequests,
                               this, std::placeholders::_1), m_property);

    for (size_t index = 1; m_interfaces.size() > 1 && index < m_interfaces.size(); index++)
    {
        typedef OCStackResult (*bindInterfaceToResource)(const OCResourceHandle &,
                const std::string &);

        try
        {
            invokeocplatform(static_cast<bindInterfaceToResource>(
                                 OC::OCPlatform::bindInterfaceToResource), m_resourceHandle,
                             m_interfaces[index]);
        }
        catch (SimulatorException &e)
        {
            stop();
            throw;
        }
    }
}

void SimulatorCollectionResourceImpl::stop()
{
    std::lock_guard<std::recursive_mutex> lock(m_objectLock);
    if (!m_resourceHandle)
        return;

    typedef OCStackResult (*UnregisterResource)(const OCResourceHandle &);

    invokeocplatform(static_cast<UnregisterResource>(OC::OCPlatform::unregisterResource),
                     m_resourceHandle);

    m_resourceHandle = nullptr;
}

SimulatorResourceModel SimulatorCollectionResourceImpl::getResourceModel()
{
    std::lock_guard<std::mutex> lock(m_modelLock);
    return m_resModel;
}

void SimulatorCollectionResourceImpl::setResourceModel(const SimulatorResourceModel &resModel)
{
    std::lock_guard<std::mutex> lock(m_modelLock);
    m_resModel = resModel;
}

void SimulatorCollectionResourceImpl::setActionType(std::map<RAML::ActionType, RAML::ActionPtr> &actionType)
{
    m_actionTypes = actionType;
}

std::vector<ObserverInfo> SimulatorCollectionResourceImpl::getObserversList()
{
    return m_observersList;
}

void SimulatorCollectionResourceImpl::notify(int id)
{
    std::lock_guard<std::recursive_mutex> lock(m_objectLock);
    if (!m_resourceHandle)
        return;

    OC::ObservationIds observers {static_cast<OCObservationId>(id)};
    sendNotification(observers);
}

void SimulatorCollectionResourceImpl::notifyAll()
{
    std::lock_guard<std::recursive_mutex> lock(m_objectLock);
    if (!m_resourceHandle)
        return;

    if (!m_observersList.size())
        return;

    OC::ObservationIds observers;
    for (auto &observer : m_observersList)
        observers.push_back(observer.id);
    sendNotification(observers);
}

std::vector<std::string> SimulatorCollectionResourceImpl::getSupportedResources()
{
    return m_supportedTypes;
}

void SimulatorCollectionResourceImpl::addChildResource(SimulatorResourceSP &resource)
{
    VALIDATE_INPUT(!resource, "Invalid child resource!")

    std::lock_guard<std::mutex> lock(m_childResourcesLock);
    if (m_childResources.end() != m_childResources.find(resource->getURI()))
    {
        throw SimulatorException(SIMULATOR_ERROR, "Child resource with same URI is already exisit!");
    }

    m_childResources[resource->getURI()] = resource;
    addLink(resource);

    // Notify application and observers
    if (m_modelCallback)
        m_modelCallback(m_uri, m_resModel);
    notifyAll();
}

void SimulatorCollectionResourceImpl::removeChildResource(SimulatorResourceSP &resource)
{
    VALIDATE_INPUT(!resource, "Invalid child resource!")

    std::lock_guard<std::mutex> lock(m_childResourcesLock);
    if (m_childResources.end() == m_childResources.find(resource->getURI()))
    {
        throw SimulatorException(SIMULATOR_ERROR, "Child resource not found in collection!");
    }

    removeLink(resource->getURI());
    m_childResources.erase(m_childResources.find(resource->getURI()));

    // Notify application and observers
    if (m_modelCallback)
        m_modelCallback(m_uri, m_resModel);
    notifyAll();
}

void SimulatorCollectionResourceImpl::removeChildResource(const std::string &uri)
{
    VALIDATE_INPUT(uri.empty(), "Uri is empty!")

    std::lock_guard<std::mutex> lock(m_childResourcesLock);
    if (m_childResources.end() == m_childResources.find(uri))
    {
        throw SimulatorException(SIMULATOR_ERROR, "Child resource not found in collection!");
    }

    removeLink(uri);
    m_childResources.erase(m_childResources.find(uri));

    // Notify application and observers
    if (m_modelCallback)
        m_modelCallback(m_uri, m_resModel);
    notifyAll();
}

std::vector<SimulatorResourceSP> SimulatorCollectionResourceImpl::getChildResources()
{
    std::lock_guard<std::mutex> lock(m_childResourcesLock);

    std::vector<SimulatorResourceSP> result;
    for (auto &entry : m_childResources)
        result.push_back(entry.second);

    return result;
}

OCEntityHandlerResult SimulatorCollectionResourceImpl::handleRequests(
    std::shared_ptr<OC::OCResourceRequest> request)
{
    if (!request)
        return OC_EH_ERROR;

    if (OC::RequestHandlerFlag::RequestFlag & request->getRequestHandlerFlag())
    {
        {
            OC::OCRepresentation rep = request->getResourceRepresentation();
            std::string payload = getPayloadString(rep);
            SIM_LOG(ILogger::INFO, "[" << m_name << "] " << request->getRequestType()
                    << " request received. \n**Payload details**\n" << payload)
        }

        // Handover the request to appropriate interface handler
        std::string interfaceType(OC::DEFAULT_INTERFACE);
        OC::QueryParamsMap queryParams = request->getQueryParameters();
        if (queryParams.end() != queryParams.find("if"))
            interfaceType = queryParams["if"];

        std::shared_ptr<OC::OCResourceResponse> response;
        if (interfaceType == OC::DEFAULT_INTERFACE)
        {
            response = requestOnBaseLineInterface(request);
        }
        else if (interfaceType == OC::LINK_INTERFACE)
        {
            response = requestOnLinkListInterface(request);
        }
        else if (interfaceType == OC::BATCH_INTERFACE)
        {
            response = requestOnBatchInterface(request);
        }

        // Send response if the request handled by resource
        if (response)
        {
            if (OC_STACK_OK != OC::OCPlatform::sendResponse(response))
                return OC_EH_ERROR;
        }
        else
        {
            SIM_LOG(ILogger::ERROR, "[" << m_name << "] " << "Unsupported request received!")
            return OC_EH_ERROR;
        }
    }

    if (OC::RequestHandlerFlag::ObserverFlag & request->getRequestHandlerFlag())
    {
        if (!isObservable())
        {
            SIM_LOG(ILogger::INFO, "[" << m_uri << "] OBSERVE request received")
            SIM_LOG(ILogger::INFO, "[" << m_uri << "] Sending error as resource is in unobservable state!")
            return OC_EH_ERROR;
        }

        OC::ObservationInfo observationInfo = request->getObservationInfo();
        if (OC::ObserveAction::ObserveRegister == observationInfo.action)
        {
            SIM_LOG(ILogger::INFO, "[" << m_uri << "] OBSERVE REGISTER request received");

            ObserverInfo info {observationInfo.obsId, observationInfo.address, observationInfo.port};
            m_observersList.push_back(info);

            if (m_observeCallback)
                m_observeCallback(m_uri, ObservationStatus::REGISTER, info);
        }
        else if (OC::ObserveAction::ObserveUnregister == observationInfo.action)
        {
            SIM_LOG(ILogger::INFO, "[" << m_uri << "] OBSERVE UNREGISTER request received");

            ObserverInfo info;
            for (auto iter = m_observersList.begin(); iter != m_observersList.end(); iter++)
            {
                if ((info = *iter), info.id == observationInfo.obsId)
                {
                    m_observersList.erase(iter);
                    break;
                }
            }

            if (m_observeCallback)
                m_observeCallback(m_uri, ObservationStatus::UNREGISTER, info);
        }
    }

    return OC_EH_OK;
}

std::shared_ptr<OC::OCResourceResponse> SimulatorCollectionResourceImpl::requestOnBaseLineInterface(
    std::shared_ptr<OC::OCResourceRequest> request)
{
    std::shared_ptr<OC::OCResourceResponse> response;

    RAML::ActionType type = getActionType(request->getRequestType());

    if (!m_actionTypes.empty())
    {
        if (m_actionTypes.end() == m_actionTypes.find(type))
            return response;
    }

    if ("GET" == request->getRequestType())
    {
        // Construct the representation
        OC::OCRepresentation ocRep = m_resModel.getOCRepresentation();
        response = std::make_shared<OC::OCResourceResponse>();
        response->setErrorCode(200);
        response->setResponseResult(OC_EH_OK);
        response->setResourceRepresentation(ocRep);
        std::string resPayload = getPayloadString(ocRep);
        SIM_LOG(ILogger::INFO, "[" << m_uri <<
                "] Sending response for GET request. \n**Payload details**" << resPayload)
    }

    // TODO: Handle PUT, POST and DELETE requests

    if (response)
    {
        response->setRequestHandle(request->getRequestHandle());
        response->setResourceHandle(request->getResourceHandle());
    }

    return response;
}

std::shared_ptr<OC::OCResourceResponse> SimulatorCollectionResourceImpl::requestOnLinkListInterface(
    std::shared_ptr<OC::OCResourceRequest> request)
{
    std::lock_guard<std::mutex> lock(m_childResourcesLock);
    std::shared_ptr<OC::OCResourceResponse> response;

    RAML::ActionType type = getActionType(request->getRequestType());

    if (!m_actionTypes.empty())
    {
        if (m_actionTypes.end() == m_actionTypes.find(type))
            return response;
    }

    if ("GET" == request->getRequestType())
    {
        // Construct the representation
        OC::OCRepresentation ocRep;
        std::vector<OC::OCRepresentation> links;
        for (auto &entry : m_childResources)
        {
            OC::OCRepresentation oicLink;
            oicLink.setValue("href", entry.second->getURI());
            oicLink.setValue("rt", entry.second->getResourceType());
            oicLink.setValue("if", entry.second->getInterface()[0]);
            links.push_back(oicLink);
        }

        ocRep.setValue("links", links);

        response = std::make_shared<OC::OCResourceResponse>();
        response->setRequestHandle(request->getRequestHandle());
        response->setResourceHandle(request->getResourceHandle());
        response->setErrorCode(200);
        response->setResponseResult(OC_EH_OK);
        response->setResourceRepresentation(ocRep);
        std::string resPayload = getPayloadString(ocRep);
        SIM_LOG(ILogger::INFO, "[" << m_uri <<
                "] Sending response for GET request. \n**Payload details**" << resPayload)
    }

    return nullptr;
}

std::shared_ptr<OC::OCResourceResponse> SimulatorCollectionResourceImpl::requestOnBatchInterface(
    std::shared_ptr<OC::OCResourceRequest>)
{
    // TODO: Handle this interface
    return nullptr;
}

void SimulatorCollectionResourceImpl::sendNotification(OC::ObservationIds &observers)
{
    std::lock_guard<std::recursive_mutex> lock(m_objectLock);
    std::shared_ptr<OC::OCResourceResponse> response(new OC::OCResourceResponse());
    response->setErrorCode(200);
    response->setResponseResult(OC_EH_OK);

    OC::OCRepresentation ocRep = m_resModel.getOCRepresentation();
    response->setResourceRepresentation(ocRep, OC::DEFAULT_INTERFACE);

    typedef OCStackResult (*NotifyListOfObservers)(OCResourceHandle, OC::ObservationIds &,
            const std::shared_ptr<OC::OCResourceResponse>);

    invokeocplatform(static_cast<NotifyListOfObservers>(OC::OCPlatform::notifyListOfObservers),
                     m_resourceHandle, observers, response);
}

void SimulatorCollectionResourceImpl::addLink(SimulatorResourceSP &resource)
{
    std::lock_guard<std::mutex> lock(m_modelLock);
    if (!m_resModel.containsAttribute("links"))
        return;

    // Create new OIC Link
    SimulatorResourceModel newLink;
    newLink.add("href", resource->getURI());
    newLink.add("rt", resource->getResourceType());
    newLink.add("if", resource->getInterface()[0]);

    // Add OIC Link if it is not present
    bool found = false;
    std::vector<SimulatorResourceModel> links =
        m_resModel.get<std::vector<SimulatorResourceModel>>("links");
    for (auto &link : links)
    {
        std::string linkURI = link.get<std::string>("href");
        if (linkURI == resource->getURI())
        {
            break;
            found = true;
        }
    }

    if (false ==  found)
    {
        links.push_back(newLink);
        m_resModel.updateValue("links", links);
    }
}

void SimulatorCollectionResourceImpl::removeLink(std::string uri)
{
    std::lock_guard<std::mutex> lock(m_modelLock);
    if (!m_resModel.containsAttribute("links"))
        return;

    // Add OIC Link if it is not present
    std::vector<SimulatorResourceModel> links =
        m_resModel.get<std::vector<SimulatorResourceModel>>("links");
    for (size_t i = 0; i < links.size(); i++)
    {
        std::string linkURI = links[i].get<std::string>("href");
        if (linkURI == uri)
        {
            links.erase(links.begin() + i);
            m_resModel.updateValue("links", links);
            break;
        }
    }
}

RAML::ActionType SimulatorCollectionResourceImpl::getActionType(std::string requestType)
{
    if (!requestType.compare("GET"))
        return RAML::ActionType::GET;

    if (!requestType.compare("PUT"))
        return RAML::ActionType::PUT;

    if (!requestType.compare("POST"))
        return RAML::ActionType::POST;

    if (!requestType.compare("DELETE"))
        return RAML::ActionType::DELETE;

    return RAML::ActionType::NONE;
}
