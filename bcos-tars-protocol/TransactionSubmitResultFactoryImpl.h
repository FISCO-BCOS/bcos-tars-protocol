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
 * @brief tars implementation for TransactionSubmitResultFactory
 * @file TransactionSubmitResultFactoryImpl.h
 * @author: ancelmo
 * @date 2021-04-20
 */

#pragma once
#include "TransactionSubmitResultImpl.h"
#include <bcos-framework/interfaces/protocol/TransactionSubmitResultFactory.h>
namespace bcostars
{
namespace protocol
{
class TransactionSubmitResultFactoryImpl : public bcos::protocol::TransactionSubmitResultFactory
{
public:
    bcos::protocol::TransactionSubmitResult::Ptr createTxSubmitResult(
        bcos::protocol::BlockHeader::Ptr _blockHeader,
        bcos::crypto::HashType const& _txHash) override
    {
        auto result = std::make_shared<TransactionSubmitResultImpl>(m_cryptoSuite);
        auto inner = result->innerPointer();
        inner->txHash.assign(_txHash.begin(), _txHash.end());
        auto blockHash = _blockHeader->hash();
        inner->blockHash.assign(blockHash.begin(), blockHash.end());

        return result;
    }

    bcos::protocol::TransactionSubmitResult::Ptr createTxSubmitResult(
        bcos::crypto::HashType const& _txHash, int32_t _status) override
    {
        auto result = std::make_shared<TransactionSubmitResultImpl>(m_cryptoSuite);
        auto inner = result->innerPointer();
        inner->txHash.assign(_txHash.begin(), _txHash.end());
        inner->status = _status;

        return result;
    }

private:
    bcos::crypto::CryptoSuite::Ptr m_cryptoSuite;
};
}  // namespace protocol
}  // namespace bcostars