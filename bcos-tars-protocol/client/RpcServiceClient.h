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
 * @file RPCServiceClient.h
 * @author: ancelmo
 * @date 2021-04-20
 */

#pragma once
#include "bcos-tars-protocol/Common.h"
#include "bcos-tars-protocol/ErrorConverter.h"
#include "bcos-tars-protocol/protocol/TransactionSubmitResultImpl.h"
#include "bcos-tars-protocol/tars/RpcService.h"
#include <bcos-framework/interfaces/rpc/RPCInterface.h>
#include <bcos-framework/libutilities/Common.h>
#include <tarscpp/servant/Application.h>
#include <boost/core/ignore_unused.hpp>

namespace bcostars
{
class RpcServiceClient : public bcos::rpc::RPCInterface
{
public:
    RpcServiceClient(bcostars::RpcServicePrx _proxy) : m_proxy(_proxy) {}
    ~RpcServiceClient() override {}

    class Callback : public RpcServicePrxCallback
    {
    public:
        explicit Callback(std::function<void(bcos::Error::Ptr)> _callback)
          : RpcServicePrxCallback(), m_callback(_callback)
        {}
        ~Callback() override {}

        virtual void callback_asyncNotifyBlockNumber(const bcostars::Error& ret) override
        {
            m_callback(toBcosError(ret));
        }
        virtual void callback_asyncNotifyBlockNumber_exception(tars::Int32 ret) override
        {
            m_callback(toBcosError(ret));
        }

    private:
        std::function<void(bcos::Error::Ptr)> m_callback;
    };

    void asyncNotifyBlockNumber(std::string const& _groupID, std::string const& _nodeName,
        bcos::protocol::BlockNumber _blockNumber,
        std::function<void(bcos::Error::Ptr)> _callback) override
    {
        auto ret = checkConnection(
            c_moduleName, "asyncNotifyBlockNumber", m_proxy, [_callback](bcos::Error::Ptr _error) {
                if (_callback)
                {
                    _callback(_error);
                }
            });
        if (!ret)
        {
            return;
        }
        m_proxy->async_asyncNotifyBlockNumber(
            new Callback(_callback), _groupID, _nodeName, _blockNumber);
    }

    // TODO: remove this
    virtual void asyncNotifyTransactionResult(std::string const& _rpcID,
        const std::string_view& groupID, bcos::crypto::HashType txHash,
        bcos::protocol::TransactionSubmitResult::Ptr result) override
    {
        class Callback : public bcostars::RpcServicePrxCallback
        {
        public:
            Callback(std::function<void(bcos::Error::Ptr&&)> callback) : m_callback(callback) {}

            void callback_asyncNotifyTransactionResult(const bcostars::Error& ret) override
            {
                m_callback(toBcosError(ret));
            }
            void callback_asyncNotifyTransactionResult_exception(tars::Int32 ret) override
            {
                m_callback(toBcosError(ret));
            }

        private:
            std::function<void(bcos::Error::Ptr&&)> m_callback;
        };
        std::function<void(bcos::Error::Ptr &&)> callback = [](bcos::Error::Ptr&&) {};
        vector<tars::Char> txHashData(txHash.begin(), txHash.end());
        auto tarsTxResult =
            std::dynamic_pointer_cast<bcostars::protocol::TransactionSubmitResultImpl>(result);

        auto servicePrx = Application::getCommunicator()->stringToProxy<RpcServicePrx>(_rpcID);
        servicePrx->async_asyncNotifyTransactionResult(new Callback(callback), _rpcID,
            std::string(groupID), txHashData, tarsTxResult->inner());
    }


    void asyncNotifyGroupInfo(bcos::group::GroupInfo::Ptr _groupInfo,
        std::function<void(bcos::Error::Ptr&&)> _callback) override
    {
        class Callback : public bcostars::RpcServicePrxCallback
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
        auto ret = checkConnection(
            c_moduleName, "asyncNotifyGroupInfo", m_proxy, [_callback](bcos::Error::Ptr _error) {
                if (_callback)
                {
                    _callback(std::move(_error));
                }
            });
        if (!ret)
        {
            return;
        }
        auto tarsGroupInfo = toTarsGroupInfo(_groupInfo);
        m_proxy->async_asyncNotifyGroupInfo(new Callback(_callback), tarsGroupInfo);
    }

    void asyncNotifyAMOPMessage(int16_t _type, std::string const& _topic, bcos::bytesConstRef _data,
        std::function<void(bcos::Error::Ptr&& _error, bcos::bytesPointer _responseData)> _callback)
        override
    {
        class Callback : public bcostars::RpcServicePrxCallback
        {
        public:
            Callback(std::function<void(bcos::Error::Ptr&&, bcos::bytesPointer)> callback)
              : m_callback(callback)
            {}

            void callback_asyncNotifyAMOPMessage(
                const bcostars::Error& ret, const vector<tars::Char>& _responseData) override
            {
                auto responseData =
                    std::make_shared<bcos::bytes>(_responseData.begin(), _responseData.end());
                m_callback(toBcosError(ret), responseData);
            }
            void callback_asyncNotifyAMOPMessage_exception(tars::Int32 ret) override
            {
                m_callback(toBcosError(ret), nullptr);
            }

        private:
            std::function<void(bcos::Error::Ptr&&, bcos::bytesPointer)> m_callback;
        };
        auto ret = checkConnection(
            c_moduleName, "asyncNotifyAMOPMessage", m_proxy, [_callback](bcos::Error::Ptr _error) {
                if (_callback)
                {
                    _callback(std::move(_error), nullptr);
                }
            });
        if (!ret)
        {
            return;
        }
        vector<tars::Char> request(_data.begin(), _data.end());
        m_proxy->async_asyncNotifyAMOPMessage(new Callback(_callback), _type, _topic, request);
    }

    bcostars::RpcServicePrx prx() { return m_proxy; }

protected:
    void start() override {}
    void stop() override {}

private:
    bcostars::RpcServicePrx m_proxy;
    std::string const c_moduleName = "RpcServiceClient";
};

}  // namespace bcostars