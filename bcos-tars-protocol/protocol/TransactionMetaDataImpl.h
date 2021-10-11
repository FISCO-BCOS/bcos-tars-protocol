/*
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
 * @brief implement for TransactionMetaData
 * @file TransactionMetaDataImpl.h
 * @author: yujiechen
 * @date: 2021-09-07
 */
#pragma once
#include "bcos-tars-protocol/tars/Block.h"
#include <bcos-framework/interfaces/crypto/KeyInterface.h>
#include <bcos-framework/interfaces/protocol/TransactionMetaData.h>

namespace bcostars
{
namespace protocol
{
class TransactionMetaDataImpl : public bcos::protocol::TransactionMetaData
{
public:
    using Ptr = std::shared_ptr<TransactionMetaDataImpl>;
    using ConstPtr = std::shared_ptr<const TransactionMetaDataImpl>;
    TransactionMetaDataImpl() : m_rawTxMetaData(std::make_shared<bcostars::TransactionMetaData>())
    {}
    TransactionMetaDataImpl(bcos::crypto::HashType const& _hash, std::string const& _to)
      : TransactionMetaDataImpl()
    {
        setHash(_hash);
        setTo(_to);
    }

    explicit TransactionMetaDataImpl(bcostars::TransactionMetaData const& _txMetaData)
      : TransactionMetaDataImpl()
    {
        bcos::crypto::HashType hashData;
        if (_txMetaData.hash.size() >= bcos::crypto::HashType::size)
        {
            hashData =
                bcos::crypto::HashType(reinterpret_cast<const bcos::byte*>(_txMetaData.hash.data()),
                    bcos::crypto::HashType::size);
        }
        setHash(hashData);
        setTo(_txMetaData.to);
    }

    ~TransactionMetaDataImpl() override {}

    bcos::crypto::HashType const& hash() const override
    {
        if (m_hash != bcos::crypto::HashType())
        {
            return m_hash;
        }
        auto const& hash = m_rawTxMetaData->hash;
        if (hash.size() >= bcos::crypto::HashType::size)
        {
            m_hash = bcos::crypto::HashType(
                reinterpret_cast<const bcos::byte*>(hash.data()), bcos::crypto::HashType::size);
        }
        return m_hash;
    }

    std::string_view to() const override { return m_rawTxMetaData->to; }

    void setHash(bcos::crypto::HashType const& _hash) override
    {
        m_hash = _hash;
        m_rawTxMetaData->hash.assign(_hash.begin(), _hash.end());
    }
    void setTo(std::string const& _to) override { m_rawTxMetaData->to = _to; }

    std::shared_ptr<bcostars::TransactionMetaData> rawTxMetaData() { return m_rawTxMetaData; }

private:
    std::shared_ptr<bcostars::TransactionMetaData> m_rawTxMetaData;
    mutable bcos::crypto::HashType m_hash = bcos::crypto::HashType();
};
}  // namespace protocol
}  // namespace bcostars
