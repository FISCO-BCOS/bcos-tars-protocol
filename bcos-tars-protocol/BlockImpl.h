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
 * @brief tars implementation for Block
 * @file BlockImpl.h
 * @author: ancelmo
 * @date 2021-04-20
 */
#pragma once
#include "Block.h"
#include "BlockHeaderImpl.h"
#include "Common.h"
#include "TransactionImpl.h"
#include "TransactionMetaDataImpl.h"
#include "TransactionReceiptImpl.h"
#include <bcos-framework/interfaces/crypto/CryptoSuite.h>
#include <bcos-framework/interfaces/protocol/Block.h>
#include <bcos-framework/interfaces/protocol/BlockHeader.h>
#include <gsl/span>
#include <memory>

namespace bcostars
{
namespace protocol
{
class BlockImpl : public bcos::protocol::Block, public std::enable_shared_from_this<BlockImpl>
{
public:
    BlockImpl(bcos::protocol::TransactionFactory::Ptr _transactionFactory,
        bcos::protocol::TransactionReceiptFactory::Ptr _receiptFactory)
      : bcos::protocol::Block(_transactionFactory, _receiptFactory),
        m_inner(std::make_shared<bcostars::Block>())
    {
        m_blockHeader = std::make_shared<bcostars::protocol::BlockHeaderImpl>(
            m_transactionFactory->cryptoSuite(),
            [m_inner = this->m_inner]() mutable { return &m_inner->blockHeader; });
    }

    ~BlockImpl() override{};

    void decode(bcos::bytesConstRef _data, bool _calculateHash, bool _checkSig) override;
    void encode(bcos::bytes& _encodeData) const override;

    int32_t version() const override { return m_inner->blockHeader.version; }
    void setVersion(int32_t _version) override { m_inner->blockHeader.version = _version; }

    bcos::protocol::BlockType blockType() const override
    {
        return (bcos::protocol::BlockType)m_inner->type;
    }

    bcos::protocol::BlockHeader::Ptr blockHeader() override { return m_blockHeader; };
    bcos::protocol::BlockHeader::ConstPtr blockHeaderConst() const override
    {
        return m_blockHeader;
    }
    bcos::protocol::Transaction::ConstPtr transaction(size_t _index) const override;
    bcos::protocol::TransactionReceipt::ConstPtr receipt(size_t _index) const override;

    // get transaction metaData
    bcos::protocol::TransactionMetaData::ConstPtr transactionMetaData(size_t _index) const override;
    void setBlockType(bcos::protocol::BlockType _blockType) override
    {
        m_inner->type = (int32_t)_blockType;
    }

    // set blockHeader
    void setBlockHeader(bcos::protocol::BlockHeader::Ptr _blockHeader) override;

    void setTransaction(size_t _index, bcos::protocol::Transaction::Ptr _transaction) override
    {
        m_inner->transactions[_index] =
            std::dynamic_pointer_cast<bcostars::protocol::TransactionImpl>(_transaction)->inner();
    }
    void appendTransaction(bcos::protocol::Transaction::Ptr _transaction) override
    {
        m_inner->transactions.emplace_back(
            std::dynamic_pointer_cast<bcostars::protocol::TransactionImpl>(_transaction)->inner());
    }

    void setReceipt(size_t _index, bcos::protocol::TransactionReceipt::Ptr _receipt) override;
    void appendReceipt(bcos::protocol::TransactionReceipt::Ptr _receipt) override;

    void appendTransactionMetaData(bcos::protocol::TransactionMetaData::Ptr _txMetaData) override;

    // get transactions size
    size_t transactionsSize() const override { return m_inner->transactions.size(); }
    size_t transactionsMetaDataSize() const override;
    // get receipts size
    size_t receiptsSize() const override { return m_inner->receipts.size(); }

    void setNonceList(bcos::protocol::NonceList const& _nonceList) override;
    void setNonceList(bcos::protocol::NonceList&& _nonceList) override;
    bcos::protocol::NonceList const& nonceList() const override;

    const bcostars::Block& inner() const { return *m_inner; }
    void setInner(const bcostars::Block& inner) { *m_inner = inner; }
    void setInner(bcostars::Block&& inner) { *m_inner = std::move(inner); }

private:
    std::shared_ptr<bcostars::Block> m_inner;
    std::shared_ptr<BlockHeaderImpl> m_blockHeader;
    mutable bcos::protocol::NonceList m_nonceList;
};
}  // namespace protocol
}  // namespace bcostars