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
 * @file GatewayServiceClient.h
 * @author: ancelmo
 * @date 2021-04-20
 */

#pragma once

#include "bcos-tars-protocol/Common.h"
#include "bcos-tars-protocol/ErrorConverter.h"
#include "bcos-tars-protocol/tars/GatewayService.h"
#include <bcos-framework/interfaces/crypto/KeyFactory.h>
#include <bcos-framework/interfaces/gateway/GatewayInterface.h>
#include <string>

#define GATEWAYCLIENT_LOG(LEVEL) BCOS_LOG(LEVEL) << "[GATEWAYCLIENT][INITIALIZER]"
#define GATEWAYCLIENT_BADGE "[GATEWAYCLIENT]"
namespace bcostars
{
class GatewayServiceClient : public bcos::gateway::GatewayInterface
{
public:
    GatewayServiceClient(
        bcostars::GatewayServicePrx _proxy, bcos::crypto::KeyFactory::Ptr keyFactory)
      : m_proxy(_proxy), m_keyFactory(keyFactory)
    {}
    GatewayServiceClient(bcostars::GatewayServicePrx _proxy) : m_proxy(_proxy) {}
    virtual ~GatewayServiceClient() {}

    void setKeyFactory(bcos::crypto::KeyFactory::Ptr keyFactory) { m_keyFactory = keyFactory; }

    void asyncSendMessageByNodeID(const std::string& _groupID, bcos::crypto::NodeIDPtr _srcNodeID,
        bcos::crypto::NodeIDPtr _dstNodeID, bcos::bytesConstRef _payload,
        bcos::gateway::ErrorRespFunc _errorRespFunc) override
    {
        class Callback : public bcostars::GatewayServicePrxCallback
        {
        public:
            Callback(bcos::gateway::ErrorRespFunc callback) : m_callback(callback) {}

            void callback_asyncSendMessageByNodeID(const bcostars::Error& ret) override
            {
                m_callback(toBcosError(ret));
            }
            void callback_asyncSendMessageByNodeID_exception(tars::Int32 ret) override
            {
                m_callback(toBcosError(ret));
            }

        private:
            bcos::gateway::ErrorRespFunc m_callback;
        };

        auto srcNodeID = _srcNodeID->data();
        auto destNodeID = _dstNodeID->data();
        m_proxy->async_asyncSendMessageByNodeID(new Callback(_errorRespFunc), _groupID,
            std::vector<char>(srcNodeID.begin(), srcNodeID.end()),
            std::vector<char>(destNodeID.begin(), destNodeID.end()),
            std::vector<char>(_payload.begin(), _payload.end()));
    }

    void asyncGetPeers(bcos::gateway::PeerRespFunc _peerRespFunc) override
    {
        class Callback : public bcostars::GatewayServicePrxCallback
        {
        public:
            Callback(bcos::gateway::PeerRespFunc callback) : m_callback(callback) {}

            void callback_asyncGetPeers(
                const bcostars::Error& ret, const std::string& _peers) override
            {
                m_callback(toBcosError(ret), _peers);
            }
            void callback_asyncGetPeers_exception(tars::Int32 ret) override
            {
                m_callback(toBcosError(ret), "");
            }

        private:
            bcos::gateway::PeerRespFunc m_callback;
        };

        auto t1 = std::chrono::high_resolution_clock::now();
        GATEWAYCLIENT_LOG(DEBUG) << LOG_BADGE("asyncGetPeers") << LOG_DESC("request");
        m_proxy->async_asyncGetPeers(
            new Callback([_peerRespFunc, t1](bcos::Error::Ptr _error, const std::string& _peers) {
                auto t2 = std::chrono::high_resolution_clock::now();
                GATEWAYCLIENT_LOG(DEBUG) << LOG_BADGE("asyncGetPeers") << LOG_KV("response", _peers)
                                         << LOG_KV("cost", (t2 - t1).count());
                _peerRespFunc(_error, _peers);
            }));
    }

    void asyncSendMessageByNodeIDs(const std::string& _groupID, bcos::crypto::NodeIDPtr _srcNodeID,
        const bcos::crypto::NodeIDs& _dstNodeIDs, bcos::bytesConstRef _payload) override
    {
        std::vector<std::vector<char>> tarsNodeIDs;
        for (auto const& it : _dstNodeIDs)
        {
            auto nodeID = it->data();
            tarsNodeIDs.emplace_back(nodeID.begin(), nodeID.end());
        }

        auto srcNodeID = _srcNodeID->data();
        m_proxy->async_asyncSendMessageByNodeIDs(nullptr, _groupID,
            std::vector<char>(srcNodeID.begin(), srcNodeID.end()), tarsNodeIDs,
            std::vector<char>(_payload.begin(), _payload.end()));
    }

    void asyncSendBroadcastMessage(const std::string& _groupID, bcos::crypto::NodeIDPtr _srcNodeID,
        bcos::bytesConstRef _payload) override
    {
        auto srcNodeID = _srcNodeID->data();
        m_proxy->async_asyncSendBroadcastMessage(nullptr, _groupID,
            std::vector<char>(srcNodeID.begin(), srcNodeID.end()),
            std::vector<char>(_payload.begin(), _payload.end()));
    }

    void asyncGetNodeIDs(
        const std::string& _groupID, bcos::gateway::GetNodeIDsFunc _getNodeIDsFunc) override
    {
        class Callback : public GatewayServicePrxCallback
        {
        public:
            Callback(
                bcos::gateway::GetNodeIDsFunc callback, bcos::crypto::KeyFactory::Ptr keyFactory)
              : m_callback(callback), m_keyFactory(keyFactory)
            {}
            void callback_asyncGetNodeIDs(
                const bcostars::Error& ret, const vector<vector<tars::Char>>& nodeIDs) override
            {
                auto bcosNodeIDs = std::make_shared<std::vector<bcos::crypto::NodeIDPtr>>();
                bcosNodeIDs->reserve(nodeIDs.size());
                for (auto const& it : nodeIDs)
                {
                    bcosNodeIDs->push_back(m_keyFactory->createKey(
                        bcos::bytesConstRef((bcos::byte*)it.data(), it.size())));
                }
                m_callback(toBcosError(ret), bcosNodeIDs);
            }
            void callback_asyncGetNodeIDs_exception(tars::Int32 ret) override
            {
                m_callback(toBcosError(ret), nullptr);
            }

        private:
            bcos::gateway::GetNodeIDsFunc m_callback;
            bcos::crypto::KeyFactory::Ptr m_keyFactory;
        };
        m_proxy->async_asyncGetNodeIDs(new Callback(_getNodeIDsFunc, m_keyFactory), _groupID);
    }

    void asyncNotifyGroupInfo(bcos::group::GroupInfo::Ptr _groupInfo,
        std::function<void(bcos::Error::Ptr&&)> _callback) override
    {
        class Callback : public bcostars::GatewayServicePrxCallback
        {
        public:
            Callback(std::function<void(bcos::Error::Ptr&&)> callback) : m_callback(callback) {}

            void callback_asyncNotifyGroupInfo(const bcostars::Error& ret) override
            {
                m_callback(toBcosError(ret));
            }
            void callback_asyncNotifyGroupInfo_exception(tars::Int32 ret) override
            {
                m_callback(toBcosError(ret));
            }

        private:
            std::function<void(bcos::Error::Ptr&&)> m_callback;
        };
        auto tarsGroupInfo = toTarsGroupInfo(_groupInfo);
        m_proxy->async_asyncNotifyGroupInfo(new Callback(_callback), tarsGroupInfo);
    }

protected:
    void start() override {}
    void stop() override {}

private:
    bcostars::GatewayServicePrx m_proxy;
    bcos::crypto::KeyFactory::Ptr m_keyFactory;
};
}  // namespace bcostars
