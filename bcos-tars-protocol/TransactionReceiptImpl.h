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
 * @file TransactionReceiptImpl.h
 * @author: ancelmo
 * @date 2021-04-20
 */

#pragma once
#include "Common.h"
#include "TransactionReceipt.h"
#include "bcos-framework/libutilities/Common.h"
#include "bcos-framework/libutilities/FixedBytes.h"
#include "interfaces/protocol/Block.h"
#include <bcos-framework/interfaces/crypto/CryptoSuite.h>
#include <bcos-framework/interfaces/crypto/Hash.h>
#include <bcos-framework/interfaces/protocol/TransactionReceipt.h>
#include <bcos-framework/libprotocol/LogEntry.h>
#include <bcos-framework/libutilities/DataConvertUtility.h>
#include <variant>

namespace bcostars
{
namespace protocol
{
class TransactionReceiptImpl : public bcos::protocol::TransactionReceipt
{
public:
    TransactionReceiptImpl() = delete;

    explicit TransactionReceiptImpl(bcos::crypto::CryptoSuite::Ptr _cryptoSuite,
        std::function<bcostars::TransactionReceipt*()> inner)
      : bcos::protocol::TransactionReceipt(_cryptoSuite), m_inner(inner)
    {}

    ~TransactionReceiptImpl() override {}
    void decode(bcos::bytesConstRef _receiptData) override;
    void encode(bcos::bytes& _encodedData) const override;

    bcos::bytesConstRef encode(bool _onlyHashFieldData = false) const override;

    int32_t version() const override { return m_inner()->version; }
    bcos::u256 const& gasUsed() const override;

    std::string_view contractAddress() const override { return m_inner()->contractAddress; }
    int32_t status() const override { return m_inner()->status; }
    bcos::bytesConstRef output() const override
    {
        return bcos::bytesConstRef(
            (const unsigned char*)m_inner()->output.data(), m_inner()->output.size());
    }
    gsl::span<const bcos::protocol::LogEntry> logEntries() const override
    {
        return gsl::span<const bcos::protocol::LogEntry>(m_logEntries.data(), m_logEntries.size());
    }
    bcos::protocol::BlockNumber blockNumber() const override { return m_inner()->blockNumber; }

    const bcostars::TransactionReceipt& inner() const { return *m_inner(); }

    void setInner(const bcostars::TransactionReceipt& inner) { *m_inner() = inner; }
    void setInner(bcostars::TransactionReceipt&& inner) { *m_inner() = std::move(inner); }

    std::function<bcostars::TransactionReceipt*()> const& innerGetter() { return m_inner; }

    void setLogEntries(std::vector<bcos::protocol::LogEntry> const& _logEntries)
    {
        m_logEntries = _logEntries;
    }

private:
    std::function<bcostars::TransactionReceipt*()> m_inner;
    mutable bcos::bytes m_buffer;
    mutable bcos::u256 m_gasUsed;
    std::vector<bcos::protocol::LogEntry> m_logEntries;
};
}  // namespace protocol
}  // namespace bcostars