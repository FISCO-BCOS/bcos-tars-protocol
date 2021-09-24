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
 * @brief tars implementation for TransactionSubmitResult
 * @file TransactionSubmitResultImpl.h
 * @author: ancelmo
 * @date 2021-04-20
 */

#pragma once

#include "Common.h"
#include "TransactionReceipt.h"
#include "TransactionReceiptImpl.h"
#include "TransactionSubmitResult.h"
#include "interfaces/crypto/CommonType.h"
#include <bcos-framework/interfaces/protocol/TransactionSubmitResult.h>
#include <bcos-framework/libutilities/Common.h>
#include <boost/lexical_cast.hpp>
#include <memory>

namespace bcostars
{
namespace protocol
{
class TransactionSubmitResultImpl : public bcos::protocol::TransactionSubmitResult
{
public:
    TransactionSubmitResultImpl() = delete;

    TransactionSubmitResultImpl(bcos::crypto::CryptoSuite::Ptr cryptoSuite)
      : bcos::protocol::TransactionSubmitResult(),
        m_cryptoSuite(cryptoSuite),
        m_inner(std::make_shared<bcostars::TransactionSubmitResult>())
    {}

    uint32_t status() const override { return m_inner->status; }
    bcos::crypto::HashType const& txHash() const override
    {
        if (!m_inner->txHash.empty())
        {
            return *(reinterpret_cast<const bcos::crypto::HashType*>(m_inner->txHash.data()));
        }
        return m_emptyHash;
    }
    bcos::crypto::HashType const& blockHash() const override
    {
        if (!m_inner->blockHash.empty())
        {
            return *(reinterpret_cast<const bcos::crypto::HashType*>(m_inner->blockHash.data()));
        }
        return m_emptyHash;
    }

    int64_t transactionIndex() const override { return m_inner->transactionIndex; }
    void setNonce(bcos::protocol::NonceType const& _nonce) override
    {
        m_inner->nonce = boost::lexical_cast<std::string>(_nonce);
    }
    bcos::protocol::NonceType const& nonce() const override
    {
        if (!m_inner->nonce.empty())
        {
            m_nonce = boost::lexical_cast<bcos::protocol::NonceType>(m_inner->nonce);
        }

        return m_nonce;
    }


    bcostars::TransactionSubmitResult const& inner() { return *m_inner; }
    void setInner(const bcostars::TransactionSubmitResult& result) { *m_inner = result; }
    void setInner(bcostars::TransactionSubmitResult&& result) { *m_inner = std::move(result); }

    std::shared_ptr<bcostars::TransactionSubmitResult> innerPointer() { return m_inner; }

private:
    mutable bcos::protocol::NonceType m_nonce;
    bcos::crypto::CryptoSuite::Ptr m_cryptoSuite;
    bcos::crypto::HashType m_emptyHash = bcos::crypto::HashType();
    std::shared_ptr<bcostars::TransactionSubmitResult> m_inner;
};
}  // namespace protocol
}  // namespace bcostars