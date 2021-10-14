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
 * @file GroupManagerServiceClient.h
 * @author: yujiechen
 * @date 2021-10-13
 */
#pragma once
#include "bcos-tars-protocol/ErrorConverter.h"
#include "bcos-tars-protocol/tars/GroupManagerService.h"
#include <bcos-framework/interfaces/multigroup/ChainNodeInfoFactory.h>
#include <bcos-framework/interfaces/multigroup/GroupInfoFactory.h>
#include <bcos-framework/interfaces/multigroup/GroupManagerInterface.h>

namespace bcostars
{
class CommonGroupMgrServiceCallback : public GroupManagerServicePrxCallback
{
public:
    CommonGroupMgrServiceCallback(std::function<void(bcos::Error::Ptr&&)> _callback)
      : m_callback(_callback)
    {}
    void callback_asyncCreateGroup(const bcostars::Error& ret) override
    {
        m_callback(toBcosError(ret));
    }
    void callback_asyncCreateGroup_exception(tars::Int32 ret) override
    {
        m_callback(toBcosError(ret));
    }

    void callback_asyncExpandGroupNode(const bcostars::Error& ret) override
    {
        m_callback(toBcosError(ret));
    }
    void callback_asyncExpandGroupNode_exception(tars::Int32 ret) override
    {
        m_callback(toBcosError(ret));
    }

    void callback_asyncRecoverGroup(const bcostars::Error& ret) override
    {
        m_callback(toBcosError(ret));
    }

    void callback_asyncRecoverGroup_exception(tars::Int32 ret) override
    {
        m_callback(toBcosError(ret));
    }

    void callback_asyncRecoverGroupNode(const bcostars::Error& ret) override
    {
        m_callback(toBcosError(ret));
    }

    void callback_asyncRecoverGroupNode_exception(tars::Int32 ret) override
    {
        m_callback(toBcosError(ret));
    }

    void callback_asyncRemoveGroup(const bcostars::Error& ret) override
    {
        m_callback(toBcosError(ret));
    }

    void callback_asyncRemoveGroup_exception(tars::Int32 ret) override
    {
        m_callback(toBcosError(ret));
    }

    void callback_asyncRemoveGroupNode(const bcostars::Error& ret) override
    {
        m_callback(toBcosError(ret));
    }

    void callback_asyncRemoveGroupNode_exception(tars::Int32 ret) override
    {
        m_callback(toBcosError(ret));
    }

    void callback_asyncStartNode(const bcostars::Error& ret) override
    {
        m_callback(toBcosError(ret));
    }

    void callback_asyncStartNode_exception(tars::Int32 ret) override
    {
        m_callback(toBcosError(ret));
    }

    void callback_asyncStopNode(const bcostars::Error& ret) override
    {
        m_callback(toBcosError(ret));
    }

    void callback_asyncStopNode_exception(tars::Int32 ret) override
    {
        m_callback(toBcosError(ret));
    }

private:
    std::function<void(bcos::Error::Ptr&&)> m_callback;
};

class GroupManagerServiceClient : public bcos::group::GroupManagerInterface
{
public:
    using Ptr = std::shared_ptr<GroupManagerServiceClient>;
    GroupManagerServiceClient(GroupManagerServicePrx _prx,
        bcos::group::ChainNodeInfoFactory::Ptr _nodeInfoFactory,
        bcos::group::GroupInfoFactory::Ptr _groupInfoFactory)
      : m_prx(_prx), m_nodeInfoFactory(_nodeInfoFactory), m_groupInfoFactory(_groupInfoFactory)
    {}
    ~GroupManagerServiceClient() override {}

    void asyncCreateGroup(bcos::group::GroupInfo::Ptr _groupInfo,
        std::function<void(bcos::Error::Ptr&&)> _callback) override;

    void asyncExpandGroupNode(std::string const& _chainID, std::string const& _groupID,
        bcos::group::ChainNodeInfo::Ptr _nodeInfo,
        std::function<void(bcos::Error::Ptr&&)> _callback) override;

    void asyncRemoveGroup(std::string const& _chainID, std::string const& _groupID,
        std::function<void(bcos::Error::Ptr&&)> _callback) override;

    void asyncRemoveGroupNode(std::string const& _chainID, std::string const& _groupID,
        std::string const& _nodeName, std::function<void(bcos::Error::Ptr&&)> _callback) override;

    void asyncRecoverGroup(std::string const& _chainID, std::string const& _groupID,
        std::function<void(bcos::Error::Ptr&&)> _callback) override;

    void asyncRecoverGroupNode(std::string const& _chainID, std::string const& _groupID,
        std::string const& _nodeName, std::function<void(bcos::Error::Ptr&&)> _callback) override;

    void asyncStartNode(std::string const& _chainID, std::string const& _groupID,
        std::string const& _nodeName, std::function<void(bcos::Error::Ptr&&)> _callback) override;

    void asyncStopNode(std::string const& _chainID, std::string const& _groupID,
        std::string const& _nodeName, std::function<void(bcos::Error::Ptr&&)> _callback) override;

    void asyncGetChainList(
        std::function<void(bcos::Error::Ptr&&, std::set<std::string>&&)> _onGetChainList) override;

    void asyncGetGroupList(std::string const& _chainID,
        std::function<void(bcos::Error::Ptr&&, std::set<std::string>&&)> _onGetGroupList) override;

    void asyncGetGroupInfo(std::string const& _chainID, std::string const& _groupID,
        std::function<void(bcos::Error::Ptr&&, bcos::group::GroupInfo::Ptr&&)> _onGetGroupInfo)
        override;

    void asyncGetNodeInfo(std::string const& _chainID, std::string const& _groupID,
        std::string const& _nodeName,
        std::function<void(bcos::Error::Ptr&&, bcos::group::ChainNodeInfo::Ptr&&)> _onGetNodeInfo)
        override;

    void asyncGetGroupInfos(std::string const& _chainID, std::vector<std::string> const& _groupList,
        std::function<void(bcos::Error::Ptr&&, std::vector<bcos::group::GroupInfo::Ptr>&&)>
            _onGetNodeInfos) override;

private:
    GroupManagerServicePrx m_prx;
    bcos::group::ChainNodeInfoFactory::Ptr m_nodeInfoFactory;
    bcos::group::GroupInfoFactory::Ptr m_groupInfoFactory;
};
}  // namespace bcostars