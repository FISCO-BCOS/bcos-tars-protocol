/**
 *  Copyright (C) 2021 FISCO BCOS.
 *  SPDX-License-Identifier: Apache-2.0
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 * @brief tars implementation of GroupManagerInterface
 * @file GroupManagerServiceClient.cpp
 * @author: yujiechen
 * @date 2021-10-13
 */
#include "GroupManagerServiceClient.h"
#include "bcos-tars-protocol/Common.h"
using namespace bcostars;

void GroupManagerServiceClient::asyncCreateGroup(
    bcos::group::GroupInfo::Ptr _groupInfo, std::function<void(bcos::Error::Ptr&&)> _callback)
{
    m_prx->async_asyncCreateGroup(
        new CommonGroupMgrServiceCallback(_callback), toTarsGroupInfo(_groupInfo));
}

void GroupManagerServiceClient::asyncExpandGroupNode(std::string const& _chainID,
    std::string const& _groupID, bcos::group::ChainNodeInfo::Ptr _nodeInfo,
    std::function<void(bcos::Error::Ptr&&)> _callback)
{
    m_prx->async_asyncExpandGroupNode(new CommonGroupMgrServiceCallback(_callback), _chainID,
        _groupID, toTarsChainNodeInfo(_nodeInfo));
}

void GroupManagerServiceClient::asyncRemoveGroup(std::string const& _chainID,
    std::string const& _groupID, std::function<void(bcos::Error::Ptr&&)> _callback)
{
    m_prx->async_asyncRemoveGroup(new CommonGroupMgrServiceCallback(_callback), _chainID, _groupID);
}

void GroupManagerServiceClient::asyncRemoveGroupNode(std::string const& _chainID,
    std::string const& _groupID, std::string const& _nodeName,
    std::function<void(bcos::Error::Ptr&&)> _callback)
{
    m_prx->async_asyncRemoveGroupNode(
        new CommonGroupMgrServiceCallback(_callback), _chainID, _groupID, _nodeName);
}

void GroupManagerServiceClient::asyncRecoverGroup(std::string const& _chainID,
    std::string const& _groupID, std::function<void(bcos::Error::Ptr&&)> _callback)
{
    m_prx->async_asyncRecoverGroup(
        new CommonGroupMgrServiceCallback(_callback), _chainID, _groupID);
}

void GroupManagerServiceClient::asyncRecoverGroupNode(std::string const& _chainID,
    std::string const& _groupID, std::string const& _nodeName,
    std::function<void(bcos::Error::Ptr&&)> _callback)
{
    m_prx->async_asyncRecoverGroupNode(
        new CommonGroupMgrServiceCallback(_callback), _chainID, _groupID, _nodeName);
}

void GroupManagerServiceClient::asyncStartNode(std::string const& _chainID,
    std::string const& _groupID, std::string const& _nodeName,
    std::function<void(bcos::Error::Ptr&&)> _callback)
{
    m_prx->async_asyncStartNode(
        new CommonGroupMgrServiceCallback(_callback), _chainID, _groupID, _nodeName);
}

void GroupManagerServiceClient::asyncStopNode(std::string const& _chainID,
    std::string const& _groupID, std::string const& _nodeName,
    std::function<void(bcos::Error::Ptr&&)> _callback)
{
    m_prx->async_asyncStopNode(
        new CommonGroupMgrServiceCallback(_callback), _chainID, _groupID, _nodeName);
}

void GroupManagerServiceClient::asyncGetChainList(
    std::function<void(bcos::Error::Ptr&&, std::set<std::string>&&)> _onGetChainList)
{
    class Callback : public GroupManagerServicePrxCallback
    {
    public:
        Callback(std::function<void(bcos::Error::Ptr&&, std::set<std::string>&&)> _callback)
          : m_callback(_callback)
        {}

        void callback_asyncGetChainList(
            const bcostars::Error& ret, const vector<std::string>& _chainList) override
        {
            std::set<std::string> chainList(_chainList.begin(), _chainList.end());
            m_callback(toBcosError(ret), std::move(chainList));
        }
        void callback_asyncGetChainList_exception(tars::Int32 ret) override
        {
            m_callback(toBcosError(ret), std::set<std::string>());
        }

    private:
        std::function<void(bcos::Error::Ptr&&, std::set<std::string>&&)> m_callback;
    };
    m_prx->async_asyncGetChainList(new Callback(_onGetChainList));
}

void GroupManagerServiceClient::asyncGetGroupList(std::string _chainID,
    std::function<void(bcos::Error::Ptr&&, std::set<std::string>&&)> _onGetGroupList)
{
    class Callback : public GroupManagerServicePrxCallback
    {
    public:
        Callback(std::function<void(bcos::Error::Ptr&&, std::set<std::string>&&)> _callback)
          : m_callback(_callback)
        {}

        void callback_asyncGetGroupList(
            const bcostars::Error& ret, const vector<std::string>& _groupList) override
        {
            std::set<std::string> groupList(_groupList.begin(), _groupList.end());
            m_callback(toBcosError(ret), std::move(groupList));
        }
        void callback_asyncGetGroupList_exception(tars::Int32 ret) override
        {
            m_callback(toBcosError(ret), std::set<std::string>());
        }

    private:
        std::function<void(bcos::Error::Ptr&&, std::set<std::string>&&)> m_callback;
    };
    m_prx->async_asyncGetGroupList(new Callback(_onGetGroupList), _chainID);
}

void GroupManagerServiceClient::asyncGetGroupInfo(std::string _chainID, std::string _groupID,
    std::function<void(bcos::Error::Ptr&&, bcos::group::GroupInfo::Ptr&&)> _onGetGroupInfo)
{
    class Callback : public GroupManagerServicePrxCallback
    {
    public:
        Callback(std::function<void(bcos::Error::Ptr&&, bcos::group::GroupInfo::Ptr&&)> _callback,
            bcos::group::ChainNodeInfoFactory::Ptr _nodeInfoFactory,
            bcos::group::GroupInfoFactory::Ptr _groupInfoFactory)
          : m_callback(_callback),
            m_nodeInfoFactory(_nodeInfoFactory),
            m_groupInfoFactory(_groupInfoFactory)
        {}

        void callback_asyncGetGroupInfo(
            const bcostars::Error& ret, const bcostars::GroupInfo& _groupInfo) override
        {
            m_callback(toBcosError(ret),
                toBcosGroupInfo(m_nodeInfoFactory, m_groupInfoFactory, _groupInfo));
        }
        void callback_asyncGetGroupInfo_exception(tars::Int32 ret) override
        {
            m_callback(toBcosError(ret), nullptr);
        }

    private:
        std::function<void(bcos::Error::Ptr&&, bcos::group::GroupInfo::Ptr&&)> m_callback;
        bcos::group::ChainNodeInfoFactory::Ptr m_nodeInfoFactory;
        bcos::group::GroupInfoFactory::Ptr m_groupInfoFactory;
    };
    m_prx->async_asyncGetGroupInfo(
        new Callback(_onGetGroupInfo, m_nodeInfoFactory, m_groupInfoFactory), _chainID, _groupID);
}

void GroupManagerServiceClient::asyncGetNodeInfo(std::string _chainID, std::string _groupID,
    std::string _nodeName,
    std::function<void(bcos::Error::Ptr&&, bcos::group::ChainNodeInfo::Ptr&&)> _onGetNodeInfo)
{
    class Callback : public GroupManagerServicePrxCallback
    {
    public:
        Callback(
            std::function<void(bcos::Error::Ptr&&, bcos::group::ChainNodeInfo::Ptr&&)> _callback,
            bcos::group::ChainNodeInfoFactory::Ptr _nodeInfoFactory)
          : m_callback(_callback), m_nodeInfoFactory(_nodeInfoFactory)
        {}

        void callback_asyncGetNodeInfo(
            const bcostars::Error& ret, const bcostars::ChainNodeInfo& _chainNodeInfo) override
        {
            m_callback(toBcosError(ret), toBcosChainNodeInfo(m_nodeInfoFactory, _chainNodeInfo));
        }
        void callback_asyncGetNodeInfo_exception(tars::Int32 ret) override
        {
            m_callback(toBcosError(ret), nullptr);
        }

    private:
        std::function<void(bcos::Error::Ptr&&, bcos::group::ChainNodeInfo::Ptr&&)> m_callback;
        bcos::group::ChainNodeInfoFactory::Ptr m_nodeInfoFactory;
    };
    m_prx->async_asyncGetNodeInfo(
        new Callback(_onGetNodeInfo, m_nodeInfoFactory), _chainID, _groupID, _nodeName);
}
