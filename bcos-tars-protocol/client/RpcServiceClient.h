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
#include "bcos-tars-protocol/tars/RpcService.h"
#include <bcos-framework/interfaces/rpc/RPCInterface.h>
#include <bcos-framework/libutilities/Common.h>
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
        m_proxy->async_asyncNotifyBlockNumber(
            new Callback(_callback), _groupID, _nodeName, _blockNumber);
    }

    virtual void asyncNotifyTransactionResult(const std::string_view& groupID,
        bcos::crypto::HashType txHash, bcos::protocol::TransactionSubmitResult::Ptr result) override
    {
        // FIXME: to impl!
        boost::ignore_unused(groupID, txHash, result);
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
        auto tarsGroupInfo = toTarsGroupInfo(_groupInfo);
        m_proxy->async_asyncNotifyGroupInfo(new Callback(_callback), tarsGroupInfo);
    }

    // TODO: implement this
    void asyncNotifyAMOPMessage(int16_t, std::string const&, bcos::bytesConstRef,
        std::function<void(bcos::Error::Ptr&& _error, bcos::bytesPointer _responseData)>) override
    {}

protected:
    void start() override {}
    void stop() override {}

private:
    bcostars::RpcServicePrx m_proxy;
};

}  // namespace bcostars