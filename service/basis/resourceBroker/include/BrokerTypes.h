//******************************************************************
//
// Copyright 2015 Samsung Electronics All Rights Reserved.
//
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

#ifndef BROKERTYPES_H_
#define BROKERTYPES_H_

#include <iostream>
#include <functional>

#include "OCPlatform.h"
#include "octypes.h"
#include "logger.h"

#include "PrimitiveResource.h"

#define BROKER_TAG PCF("BROKER")

#define BROKER_TRANSPORT OCConnectivityType::OC_IPV4


struct BrokerRequesterInfo;
class ResourcePresence;
class DevicePresence;

enum class DISCOVER_TRANSACTION;
enum class BROKER_STATE;

typedef std::function<void(std::shared_ptr<OC::OCResource>)> FindCB;
typedef std::function<DISCOVER_TRANSACTION(std::shared_ptr<PrimitiveResource> resource)> DiscoverCB;

typedef std::function<OCStackResult(BROKER_STATE)> BrokerCB;

typedef std::shared_ptr<PrimitiveResource> PrimitiveResourcePtr;
typedef std::shared_ptr<BrokerRequesterInfo> BrokerRequesterInfoPtr;

typedef std::shared_ptr<ResourcePresence> ResourcePresencePtr;
typedef std::shared_ptr<DevicePresence> DevicePresencePtr;

/*
 * @BROKER_STATE
 * brief : resourcePresence state
 * ALIVE       - It means that 'getCB' function receives 'OK' message
 * REQUESTED   - It means that broker receives the request for presence checking
 * LOST_SIGNAL - In case that 'getCB' function receives the message except 'OK'
 * DESTROYED   - In case that the presence checking is dismissed for the resource ,
 *               or there is no matched value in the Broker Callback list
 * NONE        - To be determined.
 */
enum class BROKER_STATE
{
    ALIVE = 0,
    REQUESTED,
    LOST_SIGNAL,
    DESTROYED,
    NONE
};
/*
 * @DEVICE_STATE
 * brief : devicePresence state
 * ALIVE       - It means that 'subscribeCB' function receives 'OK' message
 * REQUESTED   - It means that broker receives the request for presence checking
 * LOST_SIGNAL - In case that 'subscribeCB' function receives the message except 'OK'
 */
enum class DEVICE_STATE
{
    ALIVE = 0,
    REQUESTED,
    LOST_SIGNAL
};


struct BrokerRequesterInfo
{
    BrokerCB brockerCB;
};

typedef OC::OCPlatform::OCPresenceHandle BasePresenceHandle;
typedef std::function<void(OCStackResult, const unsigned int,
        const std::string&)> SubscribeCallback;

typedef std::function<void(const HeaderOptions&, const ResponseStatement&, int)> GetCallback;
typedef std::function<void(int)> TimeoutCallback;

#endif // BROKERTYPES_H_