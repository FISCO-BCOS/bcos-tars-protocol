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
 * @brief implementation for Block
 * @file BlockImpl.cpp
 * @author: ancelmo
 * @date 2021-04-20
 */
#include "BlockImpl.h"
using namespace bcostars;
using namespace bcostars::protocol;

void BlockImpl::decode(bcos::bytesConstRef _data, bool, bool)
{
    tars::TarsInputStream<tars::BufferReader> input;
    input.setBuffer((const char*)_data.data(), _data.size());

    m_inner->readFrom(input);
}

void BlockImpl::encode(bcos::bytes& _encodeData) const
{
    tars::TarsOutputStream<bcostars::protocol::BufferWriterByteVector> output;

    m_inner->writeTo(output);
    output.getByteBuffer().swap(_encodeData);
}

bcos::protocol::Transaction::ConstPtr BlockImpl::transaction(size_t _index) const
{
    auto inner = std::const_pointer_cast<bcostars::Block>(m_inner);
    return std::make_shared<bcostars::protocol::TransactionImpl>(
        m_transactionFactory->cryptoSuite(),
        [m_inner = this->m_inner, _index]() { return &(m_inner->transactions[_index]); });
}

bcos::protocol::TransactionReceipt::ConstPtr BlockImpl::receipt(size_t _index) const
{
    auto inner = m_inner;

    return std::make_shared<bcostars::protocol::TransactionReceiptImpl>(
        m_transactionFactory->cryptoSuite(),
        [m_inner = this->m_inner, _index]() { return &(m_inner->receipts[_index]); });
};

void BlockImpl::setBlockHeader(bcos::protocol::BlockHeader::Ptr _blockHeader)
{
    if (_blockHeader)
    {
        m_inner->blockHeader =
            std::dynamic_pointer_cast<bcostars::protocol::BlockHeaderImpl>(_blockHeader)->inner();
    }
}

void BlockImpl::setReceipt(size_t _index, bcos::protocol::TransactionReceipt::Ptr _receipt)
{
    if (_index >= m_inner->receipts.size())
    {
        m_inner->receipts.resize(m_inner->transactions.size());
    }
    auto innerReceipt =
        std::dynamic_pointer_cast<bcostars::protocol::TransactionReceiptImpl>(_receipt)->inner();
    m_inner->receipts[_index] = innerReceipt;
}

void BlockImpl::appendReceipt(bcos::protocol::TransactionReceipt::Ptr _receipt)
{
    m_inner->receipts.push_back(
        std::dynamic_pointer_cast<bcostars::protocol::TransactionReceiptImpl>(_receipt)->inner());
}

void BlockImpl::setNonceList(bcos::protocol::NonceList const& _nonceList)
{
    m_inner->nonceList.clear();
    m_inner->nonceList.reserve(_nonceList.size());
    for (auto const& it : _nonceList)
    {
        m_inner->nonceList.emplace_back(boost::lexical_cast<std::string>(it));
    }

    m_nonceList.clear();
}

void BlockImpl::setNonceList(bcos::protocol::NonceList&& _nonceList)
{
    m_inner->nonceList.clear();
    m_inner->nonceList.reserve(_nonceList.size());
    for (auto const& it : _nonceList)
    {
        m_inner->nonceList.emplace_back(boost::lexical_cast<std::string>(it));
    }

    m_nonceList.clear();
}
bcos::protocol::NonceList const& BlockImpl::nonceList() const
{
    if (m_nonceList.empty())
    {
        m_nonceList.reserve(m_inner->nonceList.size());

        for (auto const& it : m_inner->nonceList)
        {
            if (it.empty())
            {
                m_nonceList.push_back(bcos::protocol::NonceType(0));
            }
            else
            {
                m_nonceList.push_back(boost::lexical_cast<bcos::u256>(it));
            }
        }
    }

    return m_nonceList;
}

bcos::protocol::TransactionMetaData::ConstPtr BlockImpl::transactionMetaData(size_t _index) const
{
    if (transactionsMetaDataSize() <= _index)
    {
        return nullptr;
    }
    auto inner = std::const_pointer_cast<bcostars::Block>(m_inner);
    auto rawTxMetaDataPointer = &(m_inner->transactionsMetaData[_index]);
    return std::make_shared<bcostars::protocol::TransactionMetaDataImpl>(rawTxMetaDataPointer);
}

void BlockImpl::appendTransactionMetaData(bcos::protocol::TransactionMetaData::Ptr _txMetaData)
{
    auto txMetaDataImpl =
        std::dynamic_pointer_cast<bcostars::protocol::TransactionMetaDataImpl>(_txMetaData);
    m_inner->transactionsMetaData.emplace_back(*(txMetaDataImpl->rawTxMetaData()));
}

size_t BlockImpl::transactionsMetaDataSize() const
{
    return m_inner->transactionsMetaData.size();
}
