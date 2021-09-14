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
 * @brief tars implementation for TransactionReceipt
 * @file TransactionReceiptImpl.cpp
 * @author: ancelmo
 * @date 2021-04-20
 */
#include "TransactionReceiptImpl.h"

using namespace bcostars;
using namespace bcostars::protocol;

void TransactionReceiptImpl::decode(bcos::bytesConstRef _receiptData)
{
    m_buffer.assign(_receiptData.begin(), _receiptData.end());

    tars::TarsInputStream<tars::BufferReader> input;
    input.setBuffer((const char*)m_buffer.data(), m_buffer.size());

    m_inner()->readFrom(input);
    for (auto& it : m_inner()->logEntries)
    {
        std::vector<bcos::h256> topics;
        for (auto& topicIt : it.topic)
        {
            topics.emplace_back((const bcos::byte*)topicIt.data(), topicIt.size());
        }
        bcos::protocol::LogEntry logEntry(bcos::bytes(it.address.begin(), it.address.end()), topics,
            bcos::bytes(it.data.begin(), it.data.end()));
        m_logEntries.emplace_back(logEntry);
    }
}

void TransactionReceiptImpl::encode(bcos::bytes& _encodedData) const
{
    tars::TarsOutputStream<bcostars::protocol::BufferWriterByteVector> output;
    m_inner()->logEntries.clear();
    for (auto& it : m_logEntries)
    {
        bcostars::LogEntry logEntry;
        logEntry.address.assign(it.address().begin(), it.address().end());
        for (auto& topicIt : it.topics())
        {
            logEntry.topic.push_back(std::vector<char>(topicIt.begin(), topicIt.end()));
        }
        logEntry.data.assign(it.data().begin(), it.data().end());

        m_inner()->logEntries.emplace_back(logEntry);
    }
    m_inner()->writeTo(output);
    output.getByteBuffer().swap(_encodedData);
}

bcos::bytesConstRef TransactionReceiptImpl::encode(bool _onlyHashFieldData) const
{
    if (m_buffer.empty())
    {
        encode(m_buffer);
    }
    return bcos::ref(m_buffer);
}

bcos::u256 const& TransactionReceiptImpl::gasUsed() const
{
    if (!m_inner()->gasUsed.empty())
    {
        m_gasUsed = boost::lexical_cast<bcos::u256>(m_inner()->gasUsed);
    }
    return m_gasUsed;
}
