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

#ifndef RESOURCE_UPDATE_AUTOMATION_H_
#define RESOURCE_UPDATE_AUTOMATION_H_

#include "simulator_single_resource.h"
#include "attribute_generator.h"
#include <thread>

class AttributeUpdateAutomation
{
    public:
        AttributeUpdateAutomation(int id, SimulatorSingleResource *resource,
                                  const SimulatorResourceModel::Attribute &attribute,
                                  AutomationType type, int interval,
                                  updateCompleteCallback callback,
                                  std::function<void (const int)> finishedCallback);

        void start();

        void stop();

    private:
        void updateAttribute();

        SimulatorSingleResource *m_resource;
        std::string m_attrName;
        AutomationType m_type;
        int m_id;
        bool m_stopRequested;
        int m_updateInterval;
        AttributeGenerator m_attributeGen;
        updateCompleteCallback m_callback;
        std::function<void (const int)> m_finishedCallback;
        std::thread *m_thread;
};

typedef std::shared_ptr<AttributeUpdateAutomation> AttributeUpdateAutomationSP;

class ResourceUpdateAutomation
{
    public:
        ResourceUpdateAutomation(int id, SimulatorSingleResource *resource,
                                 AutomationType type, int interval,
                                 updateCompleteCallback callback,
                                 std::function<void (const int)> finishedCallback);

        void start();

        void stop();

    private:
        void updateAttributes(std::vector<SimulatorResourceModel::Attribute> attributes);

        SimulatorSingleResource *m_resource;
        AutomationType m_type;
        int m_id;
        bool m_stopRequested;
        int m_updateInterval;
        updateCompleteCallback m_callback;
        std::function<void (const int)> m_finishedCallback;
        std::thread *m_thread;
};

typedef std::shared_ptr<ResourceUpdateAutomation> ResourceUpdateAutomationSP;

#endif
